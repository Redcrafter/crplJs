///<reference path="./crpl.d.ts" />

const speed = 1.0;

let active = false;
let circleId = -1;
let selecting = false;
let moving = false;
let targetX = 0;
let targetY = 0;

checkActive();

function main() {
    if (!active) {
        checkActive();
        return;
    }

    let [mx, my] = GetMouseCell();

    if (moving) {
        // Move is done
        if (IsPaused) {
            SuspendMove(1);
        }

        if (GetQueuedMoveCount() == 0) {
            moving = false;
            circleId = CreateUnit("POWERZONE", CurrentCoords);
            SetImage(Self, "placeholder", "None");
        } else {
            let [x, y] = CurrentPixelCoords;
            SetImagePosition(Self, "placeholder", targetX - x, targetY - y, 0);
        }
    }

    if (!selecting) {
        let [cx, cy] = CurrentCoords;
        if (GetMouseButtonDown(0) && cx - 1 <= mx && mx <= cx + 1 && cy - 1 <= my && my <= cy + 1) {
            selecting = true; // Select move position
            SetImage(Self, "placeholder", "Default");
            SetImageColor(Self, "placeholder", 0, 255, 0, 128);
        }
    } else {
        if (GetKeyDown("Escape")) {
            selecting = false;
            if (!moving) {
                SetImage(Self, "placeholder", "None");
            }
        } else if (GetMouseButtonDown(0)) {
            selecting = false;
            if (moving) {
                AbortMove();
            } else if (circleId != -1) {
                Destroy(circleId, 0);
            }
            moving = true;
            QueueMove(mx, my, speed);
            if (IsPaused) {
                SuspendMove(1);
            }
            [targetX, targetY] = CellToPixel(mx, my);
            let [x, y] = CurrentPixelCoords;
            SetImagePosition(Self, "placeholder", targetX - x, targetY - y, 0);
        } else {
            let [x, y] = CellToPixel(mx, my);
            let [a, b] = CurrentPixelCoords;
            SetImagePosition(Self, "placeholder", x - a, y - b, 0);
        }
    }
}

function awake() {
    OperateWhilePaused(true);
}

function checkActive() {
    if (GetUnitAttribute(Self, CONST_AMMO) == GetUnitAttribute(Self, CONST_MAXAMMO)) {
        active = true;
        circleId = CreateUnit("POWERZONE", CurrentCoords);
    }
}