declare let Self;

declare let CONST_ACPACKETREQUESTDELAY;
declare let CONST_AMMO;
declare let CONST_AMMOAC;
declare let CONST_BEAMTARGET;
declare let CONST_BUILDCOST;
declare let CONST_CANREQUESTAMMO;
declare let CONST_CELLHEIGHT;
declare let CONST_CELLWIDTH;
declare let CONST_CONNECTABLE;
declare let CONST_COORDX;
declare let CONST_COORDY;
declare let CONST_COUNTSFORVICTORY;
declare let CONST_CREATEPZ;
declare let CONST_DESTROYMODE;
declare let CONST_DESTROYONDAMAGE;
declare let CONST_HEALRATE;
declare let CONST_HEALTH;
declare let CONST_ISBUILDING;
declare let CONST_ISDESTROYED;
declare let CONST_ISLANDED;
declare let CONST_MAXAMMO;
declare let CONST_MAXAMMOAC;
declare let CONST_MAXHEALTH;
declare let CONST_NULLIFIERDAMAGEAMT;
declare let CONST_NULLIFIERDAMAGES;
declare let CONST_PACKETREQUESTDELAY;
declare let CONST_PIXELCOORDX;
declare let CONST_PIXELCOORDY;
declare let CONST_REQUESTACPACKETS;
declare let CONST_REQUESTPACKETS;
declare let CONST_SHOWAMMOACBAR;
declare let CONST_SHOWAMMOBAR;
declare let CONST_SHOWHEALTHBAR;
declare let CONST_SNIPERTARGET;
declare let CONST_SNIPERIGNORELOS;
declare let CONST_SUPPORTSDIGITALIS;
declare let CONST_TAKEMAPSPACE;
declare let CONST_THORTARGET;

declare function AddScriptToUnit(self, scriptName: string): void;
declare function GetScriptVar(self, scriptName: string, varName: string): number | string;
declare function SetScriptVar(self, scriptName: string, varName: string, value): void;

declare function SetUnitAttribute(self, name, value): void;

declare function SetImage(self, name: string, imageName: string): void;
declare function SetImageScale(self, name: string, scaleX: number, scaleY: number): void;
declare function SetImageColor(self, name: string, red: number, green: number, blue: number, alpha: number): void;

declare function delay(time: number);


//#region Movement Commands
declare function QueueMove(x: number, y: number, speed: number): void;
declare function GetQueuedMoveCount(): number;
declare function AbortMove(): void;
declare function SuspendMove(frames: number): void;
declare function ClearQueuedMoves(): void;
declare function GetTargetOffsetX(): number;
declare function GetTargetOffsetY(): number;
declare function SetTargetOffsetX(x: number): void;
declare function SetTargetOffsetY(y: number): void;
declare function GetUnitTargetOffsetX(uid: number): number;
declare function GetUnitTargetOffsetY(uid: number): number;
declare function SetUnitTargetOffsetX(uid: number, x: number): void;
declare function SetUnitTargetOffsetY(uid: number, y: number): void;
//#endregion

declare function GetMouseButton(id: 0 | 1 | 2): boolean;

declare function SetCreeper(x: number, y: number, amt: number): void;

//#region Terrain and Wall Commands
declare function GetTerrain(x: number, y: number): number;
declare function SetTerrain(x: number, y: number, height: number): void;
declare function GetTerrainOverride(x: number, y: number): number;
declare function SetTerrainOverride(x: number, y: number, height: number): void;
declare function GetWall(x: number, y: number): number;
declare function SetWall(x: number, y: number, amt: boolean): void;
declare function GetVoid(x: number, y: number): number;
declare function SetVoid(x: number, y: number): void;
declare function IsTerrainLOS(x1: number, y1: number, x2: number, y2: number): boolean;
declare function FloodFillTerrain(x: number, y: number, minHeight: number, maxHeight: number, avoidOccupied: boolean, fullCellsOnly: boolean, maxSize: number): number[];
declare function PathFindTerrain(x1: number, y1: number, x2: number, y2: number, minHeight: number, maxHeight: number, avoidOccupied: boolean, fullCellsOnly): number[];
//#endregion

// declare function GetMousePosition(): number;