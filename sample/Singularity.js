///<reference path="./crpl.d.ts" />

let active = false;

function main() {
    if(!active && GetUnitAttribute(Self, CONST_AMMO) == GetUnitAttribute(Self, CONST_MAXAMMO)) {
        EnableTowerField(32, -100000000, -100000000, 0, true);
        active = true;
        SetTimer0(300);
    }

    if(active && GetTimer0() == 0) {
        DisableTowerField();
        DamageCreeper(CurrentCoords, 9999, 10, 2000000000, false);
        Destroy(Self, 2);
    }
}
