///<reference path="../crpl.d.ts" />

// Creeper amount and anticreeper amount are stored in separate variables so you can have it produce different amounts of each.
const Camt = 30;
const ACamt = -30;
// Timers are also stored separately
const ACinterval = 30;
const Cinterval = 30;
const Startdelay = 0;
// StartState of 1 means the emitter will produce creeper initially. Value of 0 will create AC
const startState = 1;
// Thresh is how much creeper/AC must flood the emitter in order for it to flip
const Thresh = 1.0;

let Empowerment = 1;
let EmpowermentAC = 1;
let Unstopped = 1;

let Cvar = 1;
let ACvar = 1;
// Initialize the state as creeper or AC.
let state = startState;
awake();
delay(4);
let tAmount = ACamt;
let TotalPower = 1;
let NegaPower = 1;
let EmissionBonus = 0;
let multi = false;

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
            SetPopupText(`This emitter is currently slumbering. It will awake in ${rem} seconds with an output of ${Camt * max(Empowerment + NegaPower, 0.1)} Creeper. Creeper.\n\t\t\n\t\t\n\t\t`);
        } else {
            SetPopupText(`This emitter is currently slumbering. It will awake in ${rem} seconds with an output of ${-ACamt} AC.\n\t\t\n\t\t\n\t\t`);
        }
        delay(15);
    }
}

function awake() {
    SetImageColor(Self, "main", 150, 150, 150, 255);

    if (GetScriptVar(Self, "EmitterInterface.crpl", "HasEmitterInterface") == 0) {
        AddScriptToUnit(Self, "EmitterInterface.crpl");
    }

    SetScriptVar(Self, "EmitterInterface.crpl", "EmitterType", "FlipEmitter");
}

function CProd() {
    let tCAmount = Camt * max(Empowerment + NegaPower, 0.1) * Cvar * Unstopped;
    SetPopupText(`Flood me with anti-creeper to save me!\nProducing ${tCAmount} creeper every ${round(Cinterval / 30, 2) * 1000} milliseconds.\n\n`);
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

    SetPopupText(`Producing ${abs(tAmount * Unstopped)} anti-creeper every ${round(ACinterval / 30.00, 2) * 1000} milliseconds.\n\n`);
    SetUnitAttribute(Self, CONST_COUNTSFORVICTORY, false);
    SetImageColor(Self, "main", 255, 255, 255, 255);
    if (tAmount != 0) {
        SetCreeperNoLower(CurrentCoords, tAmount);
    }
    delay(ACinterval);
}