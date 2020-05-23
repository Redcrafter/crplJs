///<reference path="./crpl.d.ts" />
// 441x95
// ws = (1/24) * 441 = 18.375
// wh = (1/24) * 95  = 3.958333333333333

function awake() {
    SetScreenMode(true);
    SetScreenCoords(0.5, 0);
    EnableAlternateControlMode(true);

    SetImage(Self, "main", "Custom0pp");
    SetImageScale(Self, "main", 18.375, 3.958333333333333);
    SetImagePosition(Self, "main", -155, 47.5, 0);
    SetImageColor(Self, "main", 32, 32, 32, 220);

    addButton("test", "Custom0pp", 12, 12);
}

let lastX = 0;
let lastY = 0;

function main() {
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

    if(sx >= 12 - 374 && sx <= 36 - 374 && sy >= -36 + 94 && sy <= -12 + 94) {
        SetImageColor(Self, "test", 128, 128, 128, 255);
    } else {
        SetImageColor(Self, "test", 255, 255, 255, 255);
    }
    //#endregion
}

// Centered at top left of box
function addButton(name, imageName, x, y) {
    SetImage(Self, name, imageName);
    // SetImageScale(Self, name, 1, 1);

    // tl = (-374, 94)
    // size = (24, 24)

    // tl - size / 2 = (-350, 70)
    SetImagePosition(Self, name, x - 362, -y + 82, 0);
}