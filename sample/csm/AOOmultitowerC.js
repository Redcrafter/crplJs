///<reference path="../crpl.d.ts" />
// Unit for csm maps
// decreases power of enemy emitters

const text = "0.9";
const textSize = 0.5;

const multiplier = 0.9;
const powerConsumption = 1.0;
const powerConsumptionInterval = 30; // in frames

const CenterImage = "Custom5";
const AooImage = "Custom0_128"

let active = false;
let MultiTower = 0;
let listIndex = 0;

SetUnitAttribute(Self, CONST_AMMO, 0);
awake();

delay(5);
// There should only be one acmultiglobal
for (const item of GetCoresWithVar("Name", "acmultiglobal")) {
    MultiTower = item;
}

listIndex = GetListCount(GetScriptVar(MultiTower, "ACmultiglobal.crpl", "NegaMultiPowers"));
update();

function main() {
    let ammo = GetUnitAttribute(Self, CONST_AMMO);
    if(ammo >= GetUnitAttribute(Self, CONST_MAXAMMO) * 0.9) {
        if(!active) {
            active = true;
            SetTimer0(powerConsumptionInterval);
            update();
        }
    } else {
        if(active) {
            active = false;
            update();
        }
    }

    if(active) {
        SetImageRotation(Self, "spin", 0.10471975511965977 + GetImageRotation(Self, "spin"));

        if(GetTimer0() == 0) {
            SetTimer0(powerConsumptionInterval);
            SetUnitAttribute(Self, CONST_AMMO, max(0, ammo - powerConsumption));
        }
    }
}

function awake() {
    SetPopupText(`Charge me with ammo to reduce enemy power!\nConsumes ${powerConsumption} ammo every ${round(powerConsumptionInterval / 30.0, 1)}s\n\n\n`);

    SetImagePosition(Self, "main", 0, 0, -0.0001);

    let [cR, cG, cB, acR, acG, acB] = GetCreeperColors();
    // inner texture
    SetImage(Self, "inner", CenterImage);
    SetImageColor(Self, "inner", cR * 255, cG * 255, cB * 255, 255);

    // spin texture
    SetImage(Self, "spin", AooImage);
    SetImageScale(Self, "spin", 2, 2);

    SetText(text);
    SetTextSize(textSize);
    SetTextY(-textSize * 2);
}

function update() {
    let Powers = GetScriptVar(MultiTower, "ACmultiglobal.crpl", "NegaMultiPowers");

    if(active) {
        Powers[listIndex] = multiplier;
    } else {
        Powers[listIndex] = 1;
    }

    SetScriptVar(MultiTower, "ACmultiglobal.crpl", "NegaMultiPowers", Powers);
    SetScriptVar(MultiTower, "ACmultiglobal.crpl", "CChanged", true);
}
