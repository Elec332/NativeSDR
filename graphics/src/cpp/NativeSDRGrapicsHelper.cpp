//
// Created by Elec332 on 25/11/2021.
//

#define IM_INTERNAL

#include "NativeSDRGraphics.h"

static ImU32 BLACK = IM_COL32(0, 0, 0, 255);
static ImU32 GRAY = IM_COL32(160, 160, 160, 255);
static ImU32 WHITE = IM_COL32(255, 255, 255, 255);

double clips[] = {
        1, 1.25, 2, 2.5, 5, 10
};

static float getLineDiff(double start, double stop, float& parts, double& diff) {
    auto totalDiff = stop - start;
    auto rDiff = totalDiff / parts;
    auto mul = powl(10.0, floorl(log10l(rDiff))); //Get the nearest power of 10
    double mDiff;
    for (auto& i : clips) {
        mDiff = (double) (i * mul);
        if (rDiff < mDiff) {
            break;
        }
    }
    diff = mDiff;
    parts = (float) std::floor(totalDiff / diff) + 1;
    auto newStart = -std::remainder(start, diff);
    if (newStart < 0) {
        newStart += diff;
    }
    if (newStart >= 0.5 * diff) {
        parts--;
    }
    return (float) newStart;
}

ImVec2 ImGui::DrawChartFrame(ImVec2& start, ImVec2& end, double yStart, double yStop, const std::function<std::string(double)>& yLabel, double xStart, double xStop, const std::function<std::string(double)>& xLabel) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 sizeZero = ImGui::CalcTextSize("0");
    start.x += 6 * sizeZero.x;
    start.y += sizeZero.y;
    end.x -= sizeZero.y;
    end.y -= 3 * sizeZero.y;
    const ImVec2 size = end - start;
    const ImVec2 leftCorner = {start.x, end.y};
    ImGui::FillBox(BLACK);
//    drawList->AddRectFilled(start, end, IM_COL32(0,255, 0,128));

    ImVec2 maxHSize = ImGui::CalcTextSize(xLabel(xStop).c_str());
    auto yParts = std::floor(size.y / (sizeZero.y * 3));
    auto xParts = std::floor(size.x / (maxHSize.x * 1.5f));
    auto yPixPerUnit = size.y / (yStop - yStart);
    auto xPixPerUnit = size.x / (xStop - xStart);

    double yUnitDiff;
    auto yStartUnit = getLineDiff(yStart, yStop, yParts, yUnitDiff);
    double xUnitDiff;
    auto xStartUnit = getLineDiff(xStart, xStop, xParts, xUnitDiff);

    auto yVal = yStartUnit + yStart;
    ImVec2 sV = {start.x, end.y - (float) (yStartUnit * yPixPerUnit)};
    ImVec2 eV = {end.x, sV.y};
    ImVec2 piffy = {sizeZero.x / 2, 0};
    for (int i = 0; i < (int) yParts; ++i) {
        drawList->AddLine(sV, eV, GRAY);
        drawList->AddLine(sV - piffy, sV, WHITE);
        std::string txt = yLabel(yVal);
        ImVec2 txtSize = ImGui::CalcTextSize(txt.c_str());
        drawList->AddText({sV.x - sizeZero.x - txtSize.x, sV.y - (txtSize.y / 2)}, WHITE, txt.c_str());
        sV.y -= (float) (yUnitDiff * yPixPerUnit);
        eV.y = sV.y;
        yVal += yUnitDiff;
    }

    auto xVal = xStartUnit + xStart;
    auto off = (float) (xStartUnit * xPixPerUnit);
    sV = {start.x + off, end.y};
    if (off < 5) {
        sV.x += (float) (xUnitDiff * xPixPerUnit);
        xVal += xUnitDiff;
        xParts -= 2;
    }
    eV = {sV.x, start.y};
    piffy = {0, sizeZero.y / 2};
    for (int i = 0; i < (int) xParts; ++i) {
        if (sV.x > end.x) {
            break;
        }
        drawList->AddLine(sV, eV, GRAY);
        drawList->AddLine(sV + piffy, sV, WHITE);
        std::string txt = xLabel(xVal);
        ImVec2 txtSize = ImGui::CalcTextSize(txt.c_str());
        drawList->AddText({sV.x - (txtSize.x / 2), sV.y + sizeZero.y}, WHITE, txt.c_str());
        sV.x += (float) (xUnitDiff * xPixPerUnit);
        eV.x = sV.x;
        xVal += xUnitDiff;
    }

    drawList->AddLine(start, leftCorner, WHITE, 2);
    drawList->AddLine(leftCorner, end, WHITE, 2);
    return {(float) xPixPerUnit, (float) yPixPerUnit};
}

void drawChartLines(const ImVec2& start, const ImVec2& end, const float* points, int pointCount, const ImVec2& partsPerUnit, double yStart, ImU32 col, bool inside, ImU32 colInside, bool inverted) {
    if (pointCount < 2) {
        return;
    }
    ImVec2 ppu = partsPerUnit;
    ImVec2 zTart = start;
    zTart.x++;
    ppu.x = (end.x - zTart.x) / (float) (pointCount - 1);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    int startIndex = inverted ? pointCount - 1 : 0;
    ImVec2 poly[4] = {{zTart.x,         std::min(std::max((float) (end.y - ((points[startIndex] - yStart) * ppu.y)), zTart.y), end.y)},
                      {zTart.x - ppu.x, 0},
                      {0,               end.y},
                      {poly[0].x,       end.y}};
    int index = startIndex;
    ImVec2& lineStart = poly[0];
    ImVec2& lineEnd = poly[1];
    while (true) {
        if (inverted) {
            index--;
        } else {
            index++;
        }
        lineEnd.y = std::min(std::max(end.y - (float) ((points[index] - yStart) * ppu.y), zTart.y), end.y);
        lineEnd.x += ppu.x * 2;
        if (inside) {
            poly[2].x = lineEnd.x;
            drawList->AddConvexPolyFilled(poly, 4, colInside);
            std::swap(poly[2], poly[3]);
        }
        drawList->AddPolyline(poly, 2, col, 0, 1);
        std::swap(lineStart, lineEnd);
        if (index >= (pointCount - 1) || index <= 0) {
            break;
        }
    }
}

void ImGui::DrawChartLine(const ImVec2& start, const ImVec2& end, const float* points, int pointCount, const ImVec2& partsPerUnit, double yStart, ImU32 col, bool inverted) {
    drawChartLines(start, end, points, pointCount, partsPerUnit, yStart, col, false, 0, inverted);
}

void ImGui::DrawChartLineFilled(const ImVec2& start, const ImVec2& end, const float* points, int pointCount, const ImVec2& partsPerUnit, double yStart, ImU32 col, ImU32 colInside, bool inverted) {
    drawChartLines(start, end, points, pointCount, partsPerUnit, yStart, col, true, colInside, inverted);
}
