//
// Created by Elec332 on 05/11/2021.
//

#include <mutex>
#include <pipeline/block/schematic.h>
#include <map>
#include <impl/pipeline/wrapped_block.h>
#include <impl/pipeline/schematic_links.h>
#include <utility>
#include <fstream>
#include <future>

#define LINK_COUNTER_START 0x7FFFFFFF

class schematic_impl : public pipeline::schematic {

    static bool save(const char*, size_t, ax::NodeEditor::SaveReasonFlags reason, void* userPointer) {
        if (((schematic_impl*) userPointer)->cfg->SaveSettings ==
            nullptr) { //Workaround for wonky implementation, it does some weird caching...
            return false;
        }
        if ((reason & ax::NodeEditor::SaveReasonFlags::Position) == ax::NodeEditor::SaveReasonFlags::Position ||
            reason == ax::NodeEditor::SaveReasonFlags::None) {
            ((schematic_impl*) userPointer)->save();
            return true;
        }
        return false;
    }

public:

    schematic_impl(pipeline::node_manager* nodeManager, std::filesystem::path file)
            : nodeManager(nodeManager), path(std::move(file)) {
        cfg = new ax::NodeEditor::Config();
        cfg->SettingsFile = nullptr;
        cfg->SaveSettings = save;
        cfg->UserPointer = this;
        ctx = nullptr; //Will be initialized in load()
        linkHandler = newLinkHandler(this);
        load();
    }

    ~schematic_impl() {
        stop_();
        cfg->SaveSettings = nullptr;
        ax::NodeEditor::DestroyEditor(ctx);
        delete cfg;
    }

    bool canConnect(const wrapped_block& ab, const pipeline::block_connection_base& aOut, const wrapped_block& bb, const pipeline::block_connection_base& bIn) {
        if (linkHandler->hasConnection(ab, aOut) && !aOut->canConnectMultiple()) {
            return false;
        }
        if (linkHandler->hasConnection(bb, bIn)) {
            return false;
        }
        return aOut->getType()->equals(bIn->getType());
    }

    void stop_() { //Stop IDE from crying about virtual functions in destructors
        std::scoped_lock<std::mutex> guard(startStopMutex);
        if (running) {
            std::list<std::future<void>> joinPool;
            forEachBlock([&](const pipeline::block_data& block) {
                joinPool.push_front(std::async(std::launch::async, [block]() {
                    block->getBlock()->stop();
                }));
            });
            for (auto& a: joinPool) {
                a.wait();
            }
            running = false;
        }
    }

    void start() override {
        std::scoped_lock<std::mutex> guard(startStopMutex);
        if (!running) {
            running = true;
            forEachBlock([](const pipeline::block_data& block) {
                block->getBlock()->start();
            });
        }
    }

    void stop() override {
        stop_();
    }

    bool isStarted() override {
        std::scoped_lock<std::mutex> guard(startStopMutex);
        return running;
    }

    void save() override {
        if (loading) {
            return;
        }
        std::scoped_lock<std::mutex> guard(saveLock);
        nlohmann::json json;
        nlohmann::json nodes = nlohmann::json::array();
        nlohmann::json links = nlohmann::json::array();
        save(nodes);
        linkHandler->save(links);
        json["nodes"] = nodes;
        json["links"] = links;
        std::ofstream file(path);
        file << json;
    }

    void load() {
        if (!exists(path)) {
            return;
        }
        std::scoped_lock<std::mutex> guard(saveLock);
        loading = true;
        stop();
        blocks.clear();
        if (ctx) {
            ax::NodeEditor::DestroyEditor(ctx);
        }
        ctx = ax::NodeEditor::CreateEditor(cfg);

        std::ifstream ifs(path);
        nlohmann::json json = nlohmann::json::parse(ifs);
        nlohmann::json nodes = json["nodes"];
        nlohmann::json links = json["links"];
        load(nodes);
        linkHandler->load(links);
        loading = false;
    }

    void save(nlohmann::json& json) const {
        for (const auto& b: blocks) {
            ImVec2 v = ax::NodeEditor::GetNodePosition(b.first);
            b.second->x = v.x;
            b.second->y = v.y;
            json.push_back(b.second->toJson());
        }
    }

    void load(nlohmann::json& json) {
        ax::NodeEditor::SetCurrentEditor(getEditor());
        size_t max = 32;
        for (const auto& element: json) {
            wrapped_block wb = fromJson(element, nodeManager);
            if (wb) {
                blocks[wb->getIdInt()] = wb;
                ax::NodeEditor::SetNodePosition(wb->getId(), ImVec2(wb->x, wb->y));
                max = std::max(max, wb->getIdInt());
            }
        }
        counter = max + 32;
    }

    void forEachBlock(const std::function<void(const pipeline::block_data&)>& func) override {
        for (const auto& b: blocks) {
            func(b.second);
        }
    }

    void forEachBlock(const std::string& type, const std::function<void(const pipeline::block_data&)>& func) override {
        for (const auto& b: blocks) {
            if (b.second->getType() == type) {
                func(b.second);
            }
        }
    }

    void forEachLink(const std::function<void(const pipeline::link&)>& func) override {
        linkHandler->forEachLink(func);
    }

    void forEachFactory(const std::function<void(const std::string&)>& func) override {
        nodeManager->forEachFactory(func);
    }

    [[nodiscard]] ax::NodeEditor::EditorContext* getEditor() const override {
        return ctx;
    }

    bool addBlock(std::string name, float x, float y) override {
        pipeline::block_factory factory = nodeManager->getFactory(name);
        if (!factory) {
            return false;
        }
        wrapped_block wb = fromFactory(factory, counter++, name);
        counter += MAX_BLOCK_PINS;
        if (counter >= LINK_COUNTER_START) {
            throw std::exception("Block limit reached!");
        }
        wb->x = x;
        wb->y = y;
        blocks[wb->getIdInt()] = wb;
        ax::NodeEditor::SetCurrentEditor(getEditor());
        ax::NodeEditor::SetNodePosition(wb->getId(), ImVec2(wb->x, wb->y));
        {
            std::scoped_lock<std::mutex> guard(startStopMutex);
            if (running) {
                wb->getBlock()->start();
            }
        }
        return true;
    }

    bool deleteBlock(ax::NodeEditor::NodeId id) override {
        wrapped_block b = blocks[(size_t) id];
        linkHandler->deleteBlockLinks(b); //All links _should_ already be gone by here, this is just to make sure.
        bool ret = blocks.erase((size_t) id);
        {
            std::scoped_lock<std::mutex> guard(startStopMutex);
            if (running) {
                b->getBlock()->stop();
            }
        }
        ax::NodeEditor::DeleteNode(id);
        save();
        return ret;
    }

    pipeline::block_data getBlock(ax::NodeEditor::NodeId id) override {
        return blocks[(size_t) id];
    }

    bool deleteLink(ax::NodeEditor::LinkId id) override {
        return linkHandler->deleteLink(id);
    }

    bool canConnect(ax::NodeEditor::PinId& a, ax::NodeEditor::PinId& b) override {
        auto ai = (size_t) a;
        ai = ai - (ai % 32);
        wrapped_block& ba = blocks[ai];
        auto bi = (size_t) b;
        bi = bi - (bi % 32);
        wrapped_block& bb = blocks[bi];
        if (!ba || !bb) {
            return false;
        }

        pipeline::block_connection_base oa = ba->getOutputPin(a);
        pipeline::block_connection_base ob = bb->getOutputPin(b);

        if (oa && ob) {
            return false;
        }
        if (oa || ob) {
            if (oa) {
                ob = bb->getInputPin(b);
                if (!ob) {
                    return false;
                }
                return canConnect(ba, oa, bb, ob);
            } else {
                oa = ba->getInputPin(a);
                if (!oa) {
                    return false;
                }
                {
                    ax::NodeEditor::PinId tmp = a;
                    a = b;
                    b = tmp;
                }
                return canConnect(bb, ob, ba, oa);
            }
        } else {
            return false;
        }
    }

    bool connect(ax::NodeEditor::PinId a, ax::NodeEditor::PinId b) override {
        if (a == b || !canConnect(a, b)) {
            return false;
        }
        auto nid = (size_t) a;
        nid = nid - (nid % 32);
        wrapped_block block = blocks[nid];
        if (!block) {
            return false;
        }
        nid = (size_t) b;
        nid = nid - (nid % 32);
        wrapped_block block_b = blocks[nid];
        if (!block_b) {
            return false;
        }
        return linkHandler->doConnect(a, block.get(), b, block_b.get());
    }

private:

    std::mutex saveLock;
    bool loading = false;
    std::filesystem::path path;
    std::shared_ptr<schematic_link_handler> linkHandler;
    pipeline::node_manager* nodeManager;
    size_t counter = 32;
    ax::NodeEditor::Config* cfg;
    ax::NodeEditor::EditorContext* ctx;
    std::map<size_t, wrapped_block> blocks;
    bool running = false;
    std::mutex startStopMutex;

};

pipeline::schematic* pipeline::newSchematic(pipeline::node_manager* nodeManager, const std::filesystem::path& file) {
    return new schematic_impl(nodeManager, file);
}

void pipeline::deleteSchematic(pipeline::schematic* schematic) {
    delete (schematic_impl*) schematic;
}

class schematic_link_handler_impl : public schematic_link_handler {

public:

    explicit schematic_link_handler_impl(pipeline::schematic* schematic) : schematic(schematic) {
    }

    void save(nlohmann::json& json) const override {
        for (const auto& b: links) {
            nlohmann::json lj;
            lj["a"] = (size_t) b.second->startPin;
            lj["b"] = (size_t) b.second->endPin;
            json.push_back(lj);
        }
    }

    void load(nlohmann::json& json) override {
        pinToLinks.clear();
        links.clear();
        linkCounter = LINK_COUNTER_START;

        ax::NodeEditor::SetCurrentEditor(schematic->getEditor());
        for (const auto& element: json) {
            auto a = element["a"].get<size_t>();
            auto b = element["b"].get<size_t>();
            pipeline::block_data blockA = schematic->getBlock(a - (a % 32));
            pipeline::block_data blockB = schematic->getBlock(b - (b % 32));
            if (blockA && blockB && ((wrapped_block_instance*) blockA.get())->getOutputPin(a) != nullptr && ((wrapped_block_instance*) blockB.get())->getInputPin(b) != nullptr) {
                doConnect(a, (wrapped_block_instance*) blockA.get(), b, (wrapped_block_instance*) blockB.get());
            }
        }
    }

    block_connection* getConnection(ax::NodeEditor::PinId pin, bool input) {
        auto id = (size_t) pin;
        auto block = (wrapped_block_instance*) schematic->getBlock(id - (id % 32)).get();
        if (!block) {
            return nullptr;
        }
        pipeline::block_connection_base ret;
        if (input) {
            ret = block->getInputPin(id);
        } else {
            ret = block->getOutputPin(id);
        }
        if (ret) {
            return (block_connection*) ret.get();
        } else {
            return nullptr;
        }
    }

    bool doConnect(ax::NodeEditor::PinId pinA, wrapped_block_instance* blockA, ax::NodeEditor::PinId pinB, wrapped_block_instance* blockB) override {
        if (linkCounter >= 0xFFFFFFFE) {
            throw std::exception("Link overflow!");
        }
        if (pinA == pinB) {
            throw std::exception("WHAT");
        }
        size_t id = linkCounter++;
        links[id] = std::make_shared<pipeline::link_instance>(id, pinA, pinB);
        std::list<size_t>& pins = pinToLinks[(size_t) pinA];
        pins.push_front(id);
        pinToLinks[(size_t) pinB].push_front(id);

        auto output = ((block_connection*) blockA->getOutputPin(pinA).get());
        output->initOutput(this, pinA);
        output->setConnectionCount(pins.size());

        ((block_connection*) blockB->getInputPin(pinB).get())->setObject(output, 0);

        schematic->save();
        return true;
    }

    bool deleteLink(ax::NodeEditor::LinkId id_) override {
        auto id = (size_t) id_;
        {
            pipeline::link l = links[id];
            std::list<size_t>& pins = pinToLinks[(size_t) l->startPin];
            pins.remove(id);
            pinToLinks[(size_t) l->endPin].remove(id);
            getConnection(l->endPin, true)->setObject(nullptr, 0);
            getConnection(l->startPin, false)->setConnectionCount(pins.size());
        }
        bool ret = links.erase((size_t) id);
        ax::NodeEditor::DeleteLink(id);
        schematic->save();
        return ret;
    }

    void deleteLinks(const wrapped_block& block, const pipeline::block::connection_list& list) {
        for (const auto& conn: list) {
            size_t id = conn->getId() + block->getIdInt();
            for (auto i: pinToLinks[id]) {
                {
                    pipeline::link l = links[i];
                    if (l) {
                        auto pin = (size_t) l->startPin;
                        if (id == pin) {
                            pinToLinks[(size_t) l->endPin].remove(i);
                            getConnection(l->endPin, true)->setObject(nullptr, 0);
                        } else {
                            std::list<size_t>& pins = pinToLinks[pin];
                            pins.remove(i);
                            getConnection(l->startPin, false)->setConnectionCount(pins.size());
                        }
                    }
                }
                links.erase(i);
                ax::NodeEditor::DeleteLink(i);
            }
            pinToLinks.erase(id);
        }
    }

    void deleteBlockLinks(const wrapped_block& block) override {
        deleteLinks(block, block->getBlock()->getOutputs());
        deleteLinks(block, block->getBlock()->getInputs());
        schematic->save();
    }

    bool hasConnection(const wrapped_block& block, const pipeline::block_connection_base& blockConnection) override {
        return !pinToLinks[block->getIdInt() + blockConnection->getId()].empty();
    }

    void forEachLink(const std::function<void(const pipeline::link&)>& func) override {
        for (const auto& p: links) {
            func(p.second);
        }
    }

    void onLinkValueChanged(ax::NodeEditor::PinId pin, const block_connection* newRef, int flags) override {
        for (const auto& l: pinToLinks[(size_t) pin]) {
            getConnection(links[l]->endPin, true)->setObject(newRef, flags);
        }
    }

private:

    std::map<size_t, std::list<size_t>> pinToLinks;
    std::map<size_t, pipeline::link> links;

    pipeline::schematic* schematic;
    size_t linkCounter = LINK_COUNTER_START;

};

std::shared_ptr<schematic_link_handler> newLinkHandler(pipeline::schematic* schematic) {
    return std::make_shared<schematic_link_handler_impl>(schematic);
}

