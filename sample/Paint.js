///<reference path="./crpl.d.ts" />
// 441x95
// ws = (1/24) * 441 = 18.375
// wh = (1/24) * 95  = 3.958333333333333

// awake();

// Centered at top left of box
function addButton(name, imageName, x, y, w, h) {
    SetImage(Self, name, imageName);
    SetImageScale(Self, name, w / 24, h / 24);
    SetImagePosition(Self, name, x + (tlx + w / 2), -y + (tly - h / 2), -1);
}

function awake() {
    SetScreenMode(true);
    SetScreenCoords(0.5, 0);
    EnableAlternateControlMode(true);

    // Custom0pp has to be pure white image
    SetImage(Self, "main", "Custom0pp");
    SetImageScale(Self, "main", 18.375, 3.958333333333333);
    SetImagePosition(Self, "main", -155, 47.5, 0);
    SetImageColor(Self, "main", 32, 32, 32, 220);

    addButton("test", "Custom0pp", 12, 12, 72, 72);
    addButton("test1", "Custom0pp", 96, 12, 72, 72);
}

let lastX = 0;
let lastY = 0;

function main() {
    awake();

    //#region scrolling
    let dx = 0;
    let dy = 0;

    if(GetMouseButton(1)) {
        let [mx, my] = GetMousePosition();
        dx = lastX - mx;
        dy = lastY - my;
    }
    if(GetKey("W")) {
        dy = dy + 10;
    }
    if(GetKey("A")) {
        dx = dx - 10;
    }
    if(GetKey("S")) {
        dy = dy - 10;
    }
    if(GetKey("D")) {
        dx = dx + 10;
    }

    let [cx, cy] = GetCameraPosition();
    SetCameraPosition(cx + dx, cy + dy);

    [lastX, lastY] = GetMousePosition();
    //#endregion

    //#region Rect collision
    let [sx, sy] = GetMouseScreenPixelPosition();
    sx = sx - ScreenWidth / 2;

    function contains(x, y, w, h) {
        return sx >= x + tlx && sx <= x + w + tlx && sy >= tly - y - h && sy <= tly - y;
    }

    if(contains(12, 12, 72, 72)) {
        if (GetMouseButton(0)) {
            SetImageColor(Self, "test", 128, 128, 128, 255);
        } else {
            SetImageColor(Self, "test", 192, 192, 192, 255);
        }
    } else {
        SetImageColor(Self, "test", 255, 255, 255, 255);
    }

    if (contains(96, 12, 72, 72)) {
        if (GetMouseButton(0)) {
            SetImageColor(Self, "test1", 128, 128, 128, 255);
        } else {
            SetImageColor(Self, "test1", 192, 192, 192, 255);
        }
    } else {
        SetImageColor(Self, "test1", 255, 255, 255, 255);
    }
    //#endregion
}

const tlx = -374;
const tly = 94;
