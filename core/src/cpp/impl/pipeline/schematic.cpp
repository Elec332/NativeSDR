//
// Created by Elec332 on 05/11/2021.
//

#include <pipeline/block/schematic.h>
#include <map>
#include <impl/pipeline/wrapped_block.h>
#include <impl/pipeline/schematic_links.h>
#include <iostream>

#define LINK_COUNTER_START 0x7FFFFFFF

class schematic_impl : public pipeline::schematic {

public:

    explicit schematic_impl(pipeline::node_manager* nodeManager) : nodeManager(nodeManager) {
        ctx = ax::NodeEditor::CreateEditor();
        linkHandler = newLinkHandler(this);
    }

    ~schematic_impl() {
        ax::NodeEditor::DestroyEditor(ctx);
    }

    bool canConnect(const wrapped_block& ab, const pipeline::block_connection_base& aOut, const wrapped_block& bb,
                    const pipeline::block_connection_base& bIn) {
        if (linkHandler->hasConnection(ab, aOut) && !aOut->canConnectMultiple()) {
            return false;
        }
        if (linkHandler->hasConnection(bb, bIn)) {
            return false;
        }
        return true;
    }

    void forEachBlock(const std::function<void(const pipeline::block_data&)>& func) const override {
        for (const auto& b: blocks) {
            func(b.second);
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
        blocks[(size_t) wb->getId()] = wb;
        ax::NodeEditor::SetCurrentEditor(getEditor());
        ax::NodeEditor::SetNodePosition(wb->getId(), ImVec2(wb->x, wb->y));
        return true;
    }

    bool deleteBlock(ax::NodeEditor::NodeId id) override {
        wrapped_block b = blocks[(size_t) id];
        linkHandler->deleteBlockLinks(b);
        return blocks.erase((size_t) id);
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
        } else if (oa || ob) {
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
        return linkHandler->doConnect(a, block, b, block_b);
    }

private:

    std::shared_ptr<schematic_link_handler> linkHandler;
    pipeline::node_manager* nodeManager;
    size_t counter = 32;
    ax::NodeEditor::EditorContext* ctx;
    std::map<size_t, wrapped_block> blocks;

};

pipeline::schematic* pipeline::newSchematic(pipeline::node_manager* nodeManager) {
    return new schematic_impl(nodeManager);
}

void pipeline::deleteSchematic(pipeline::schematic* schematic) {
    delete (schematic_impl*) schematic;
}

class test {

    explicit test(int i) {
    }

};

class schematic_link_handler_impl : public schematic_link_handler {

public:

    explicit schematic_link_handler_impl(pipeline::schematic* schematic) {
    }

    bool doConnect(ax::NodeEditor::PinId pinA, wrapped_block& blockA, ax::NodeEditor::PinId pinB,
                   wrapped_block& blockB) override {
        if (linkCounter >= 0xFFFFFFFE) {
            throw std::exception("Link overflow!");
        }
        if (pinA == pinB) {
            throw std::exception("WHAT");
        }
        size_t id = linkCounter++;
        links[id] = std::make_shared<pipeline::link_instance>(id, pinA, pinB);
        pinToLinks[(size_t) pinA].push_front(id);
        pinToLinks[(size_t) pinB].push_front(id);
        return true;
    }

    bool deleteLink(ax::NodeEditor::LinkId id_) override {
        auto id = (size_t) id_;
        {
            pipeline::link l = links[id];
            pinToLinks[(size_t) l->startPin].remove(id);
            pinToLinks[(size_t) l->endPin].remove(id);
        }
        return links.erase((size_t) id);
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
                            pinToLinks[(size_t) l->endPin].remove(id);
                        } else {
                            pinToLinks[pin].remove(id);
                        }
                    }
                }
                links.erase(i);
            }
            pinToLinks.erase(id);
        }
    }

    void deleteBlockLinks(const wrapped_block& block) override {
        deleteLinks(block, block->getBlock()->getOutputs());
        deleteLinks(block, block->getBlock()->getInputs());
    }

    bool hasConnection(const wrapped_block& block, const pipeline::block_connection_base& blockConnection) override {
        return !pinToLinks[block->getIdInt() + blockConnection->getId()].empty();
    }

    void forEachLink(const std::function<void(const pipeline::link&)>& func) override {
        for (const auto& p: links) {
            func(p.second);
        }
    }

private:

    std::map<size_t, std::list<size_t>> pinToLinks;
    std::map<size_t, pipeline::link> links;

    size_t linkCounter = LINK_COUNTER_START;

};

std::shared_ptr<schematic_link_handler> newLinkHandler(pipeline::schematic* schematic) {
    return std::make_shared<schematic_link_handler_impl>(schematic);
}

