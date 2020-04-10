///<reference path="./crpl.d.ts" />

const Camt = 30;
const ACamt = -30;
// Timers are also stored separately
const ACinterval = 30;
const Cinterval = 30;
const Startdelay = 0;
// StartState of 1 means the emitter will produce creeper initially. Value of 0 will create AC
const startState = 1;
const Name = "FlipEmitter";
// Thresh is how much creeper/AC must flood the emitter in order for it to flip
const Thresh = 1.0;
// ImageSlot can be changed if you want to use a different slot.
const ImageSlot = "Custom0";
const Empowerment = 1;
const EmpowermentAC = 1;
const Unstopped = 1;

var Cvar = 1;
var ACvar = 1;
// Initialize the state as creeper or AC.
var state = startState;
awake();
delay(4);
var tAmount = ACamt;
var TotalPower = 1;
var EmissionBonus = 0;
var NegaPower = 0; // ?
var multi = false;

function main() {
    if (GetGameTimeFrames() > Startdelay) {
        // The timers are used to track how long since the emitter last flipped, and hide the "flip text" after 5 seconds
        if (GetTimer0() == 0 && GetTimer1() == 0) {
            SetText("");
        }

        if (state) {
            // If we're producting creeper, we first check if we've been flooded with sufficient AC to trigger the flip
            if (GetCreeper(CurrentCoords) < -Thresh) {
                // Flavor text with a timer to remove it after 5 seconds
                SetText("Thanks for rescuing me from the evil creeps!");
                SetTextColor(50, 220, 50, 255);
                SetTextY(20);
                SetTextSize(1.1);

                SetTimer1(150);
                // Swap out the state so it behaves properly on next invocation
                state = 0;
                // AC production and Creeper production have been placed in functions so they can easily be activated immediately upon flipping
                ACProd();
            } else {
                CProd();
            }
        } else {
            if (GetCreeper(CurrentCoords) > Thresh) {
                SetText("Nooo! Why did you let them capture me?!");
                SetTextColor(50, 220, 50, 255);
                SetTextY(20);
                SetTextSize(1.1);

                SetTimer0(150);
                state = 1;

                CProd();
            } else {
                ACProd();
            }
        }
    } else {
        let rem = ceil((Startdelay - GetGameTimeFrames) / 30);
        if (startState) {
            SetPopupText("This emitter is currently slumbering. It will awake in " + rem + " seconds with an output of " + (Camt * max(Empowerment + NegaPower, 0.1)) + " Creeper. Creeper.\n\t\t\n\t\t\n\t\t");
        } else {
            SetPopupText("This emitter is currently slumbering. It will awake in " + rem + " seconds with an output of " + -ACamt + " AC.\n\t\t\n\t\t\n\t\t");
        }
        delay(15);
    }
}

function awake() {
    SetUnitAttribute(Self, CONST_COUNTSFORVICTORY, 0);
    SetUnitAttribute(Self, CONST_NULLIFIERDAMAGES, 0);
    SetUnitAttribute(Self, CONST_SUPPORTSDIGITALIS, 0);

    SetImage(Self, "main", ImageSlot);
    SetImageScale(Self, "main", 0.8, 0.8);
    SetImageColor(Self, "main", 150, 150, 150, 255);

    if (GetScriptVar(Self, "EmitterInterface.crpl", "HasEmitterInterface") == 0) {
        AddScriptToUnit(Self, "EmitterInterface.crpl");
    }

    SetScriptVar(Self, "EmitterInterface.crpl", "EmitterType", Name);
    SetScriptVar(Self, "EmitterInterface.crpl", "EmitterNetName", 0);
}

function CProd() {
    let tCAmount = Camt * max(Empowerment + NegaPower, 0.1) * Cvar * Unstopped;
    SetPopupText("Flood me with anti-creeper to save me!\nProducing " + tCAmount + " creeper every " + (round(Cinterval / 30, 2) * 1000) + " milliseconds.\n\n");
    SetUnitAttribute(Self, CONST_COUNTSFORVICTORY, true);
    SetImageColor(Self, "main", 150, 150, 150, 255);

    if (tCAmount != 0) {
        SetCreeperNoLower(CurrentCoords, tCAmount);
    }
    delay(Cinterval);
}

function ACProd() {
    // Only update the tAmount value if multi has been flagged. Otherwise we stick with the base value.
    if (multi) {
        tAmount = (ACamt * TotalPower * EmpowermentAC * ACvar - EmissionBonus) * Unstopped;
    } else {
        tAmount = (ACamt * EmpowermentAC * ACvar - EmissionBonus) * Unstopped;
    }

    SetPopupText("Producing " + abs(tAmount * Unstopped) + " anti-creeper every " + (round(ACinterval / 30.00, 2) * 1000) + " milliseconds.\n\n");
    SetUnitAttribute(Self, CONST_COUNTSFORVICTORY, false);
    SetImageColor(Self, "main", 255, 255, 255, 255);
    if (tAmount != 0) {
        SetCreeperNoLower(CurrentCoords, tAmount);
    }
    delay(ACinterval);
}