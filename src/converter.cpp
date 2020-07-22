#include "logger.h"
#include "converter.h"

#include <algorithm>
#include <map>
#include <set>
#include <sstream>
#include <cmath>
#include <cassert>

// delete variables after exiting scope?
// test performance impact
// #define deleteVars

struct NativeVar {
	std::vector<Types> consumes;
	std::vector<Types> returns;
};

static std::map<std::string, NativeVar> nativeVars = {
	// "I", "J", "K"
	{"CONST_ACPACKETREQUESTDELAY", {{}, {Types::String}}},
	{"CONST_AMMO", {{}, {Types::String}}},
	{"CONST_AMMOAC", {{}, {Types::String}}},
	{"CONST_BEAMTARGET", {{}, {Types::String}}},
	{"CONST_BUILDCOST", {{}, {Types::String}}},
	{"CONST_CANREQUESTAMMO", {{}, {Types::String}}},
	{"CONST_CELLHEIGHT", {{}, {Types::String}}},
	{"CONST_CELLWIDTH", {{}, {Types::String}}},
	{"CONST_CONNECTABLE", {{}, {Types::String}}},
	{"CONST_COORDX", {{}, {Types::String}}},
	{"CONST_COORDY", {{}, {Types::String}}},
	{"CONST_COUNTSFORVICTORY", {{}, {Types::String}}},
	{"CONST_CREATEPZ", {{}, {Types::String}}},
	{"CONST_DESTROYMODE", {{}, {Types::String}}},
	{"CONST_DESTROYONDAMAGE", {{}, {Types::String}}},
	{"CONST_HEALRATE", {{}, {Types::String}}},
	{"CONST_HEALTH", {{}, {Types::String}}},
	{"CONST_ISBUILDING", {{}, {Types::String}}},
	{"CONST_ISDESTROYED", {{}, {Types::String}}},
	{"CONST_ISLANDED", {{}, {Types::String}}},
	{"CONST_MAXAMMO", {{}, {Types::String}}},
	{"CONST_MAXAMMOAC", {{}, {Types::String}}},
	{"CONST_MAXHEALTH", {{}, {Types::String}}},
	{"CONST_NULLIFIERDAMAGEAMT", {{}, {Types::String}}},
	{"CONST_NULLIFIERDAMAGES", {{}, {Types::String}}},
	{"CONST_PACKETREQUESTDELAY", {{}, {Types::String}}},
	{"CONST_PIXELCOORDX", {{}, {Types::String}}},
	{"CONST_PIXELCOORDY", {{}, {Types::String}}},
	{"CONST_REQUESTACPACKETS", {{}, {Types::String}}},
	{"CONST_REQUESTPACKETS", {{}, {Types::String}}},
	{"CONST_SHOWAMMOACBAR", {{}, {Types::String}}},
	{"CONST_SHOWAMMOBAR", {{}, {Types::String}}},
	{"CONST_SHOWHEALTHBAR", {{}, {Types::String}}},
	{"CONST_SNIPERTARGET", {{}, {Types::String}}},
	{"CONST_SNIPERIGNORELOS", {{}, {Types::String}}},
	{"CONST_SUPPORTSDIGITALIS", {{}, {Types::String}}},
	{"CONST_TAKEMAPSPACE", {{}, {Types::String}}},
	{"CONST_THORTARGET", {{}, {Types::String}}},

	#pragma region Math Commands
	{"E", {{}, {Types::Float}}},
	{"PI", {{}, {Types::Float}}},

	{"abs", {{Types::Number}, {Types::Number}}},
	{"acos", {{Types::Number}, {Types::Number}}},
	{"approximately", {{Types::Number, Types::Number}, {Types::Number}}},
	// {"asfloat", {{Types::Number}, {Types::Float}}},
	{"asin", {{Types::Number}, {Types::Number}}},
	// {"asint", {{Types::Number}, {Types::Number}}},
	{"atan", {{Types::Number}, {Types::Number}}},
	{"atan2", {{Types::Number, Types::Number}, {Types::Number}}},
	{"ceil", {{Types::Number}, {Types::Number}}},
	{"cos", {{Types::Number}, {Types::Number}}},
	{"floor", {{Types::Number}, {Types::Number}}},
	{"ln", {{Types::Number}, {Types::Number}}},
	{"log", {{Types::Number, Types::Number}, {Types::Number}}},
	{"log10", {{Types::Number}, {Types::Number}}},
	{"max", {{Types::Number, Types::Number}, {Types::Number}}},
	{"min", {{Types::Number, Types::Number}, {Types::Number}}},
	{"pow", {{Types::Number, Types::Number}, {Types::Number}}},
	{"round", {{Types::Number, Types::Number}, {Types::Number}}},
	{"ShortestAngle", {{Types::Number, Types::Number}, {Types::Number}}},
	{"sin", {{Types::Number}, {Types::Number}}},
	{"sqrt", {{Types::Number}, {Types::Number}}},
	{"tan", {{Types::Number}, {Types::Number}}},
	#pragma endregion

	#pragma region Flow Control
	{"delay", {{Types::Number}, {}}},
	#pragma endregion

	#pragma region Timers
	{"GetTimer0", {{}, {Types::Number}}},
	{"SetTimer0", {{Types::Number}, {}}},
	{"GetTimer1", {{}, {Types::Number}}},
	{"SetTimer1", {{Types::Number}, {}}},
	{"GetTimer2", {{}, {Types::Number}}},
	{"SetTimer2", {{Types::Number}, {}}},
	{"GetTimer3", {{}, {Types::Number}}},
	{"SetTimer3", {{Types::Number}, {}}},
	#pragma endregion

	#pragma region String Commands
	{"Concat", {{Types::StringNum, Types::StringNum}, {Types::String}}},
	{"Substring", {{Types::String, Types::Number, Types::Number}, {Types::String}}},
	{"StartsWith", {{Types::String, Types::String}, {Types::Boolean}}},
	{"EndsWith", {{Types::String, Types::String}, {Types::Boolean}}},
	{"Split", {{Types::String, Types::String}, {Types::List}}},
	{"StringToList", {{Types::String}, {Types::List}}},
	{"ToUpper", {{Types::String}, {Types::String}}},
	{"ToLower", {{Types::String}, {Types::String}}},
	{"StringLength", {{Types::String}, {Types::Int}}},
	{"StringReplace", {{Types::String, Types::String, Types::String}, {Types::String}}},
	#pragma endregion

	#pragma region Input Commands
	{"GetKey", {{Types::String}, {Types::Boolean}}},
	{"GetKeyDown", {{Types::String}, {Types::Boolean}}},
	{"GetKeyUp", {{Types::String}, {Types::Boolean}}},
	{"GetMouseButton", {{Types::Int}, {Types::Boolean}}},
	{"GetMouseButtonDown", {{Types::Int}, {Types::Boolean}}},
	{"GetMouseButtonUp", {{Types::Int}, {Types::Boolean}}},
	{"GetMousePosition", {{}, {Types::Float, Types::Float}}},
	{"GetMouseCell", {{}, {Types::Int, Types::Int}}},
	{"GetMouseScreenPosition", {{}, {Types::Float, Types::Float}}},
	{"GetMouseScreenPixelPosition", {{}, {Types::Float, Types::Float}}},
	{"EnableAlternateControlMode", {{Types::Boolean}, {}}},
	{"EnableNormalMouse", {{Types::Boolean}, {}}},
	{"EnableNormalKeyInput", {{Types::Boolean}, {}}},
	#pragma endregion

	#pragma region Lists
	{"GetListCount", {{Types::List}, {Types::Number}}},
	#pragma endregion

	#pragma region Movement Commands
	{"QueueMove", {{Types::Number, Types::Number, Types::Number}, {}}},
	{"GetQueuedMoveCount", {{}, {Types::Number}}},
	{"AbortMove", {{}, {}}},
	{"SuspendMove", {{Types::Number}, {}}},
	{"ClearQueuedMoves", {{}, {}}},
	{"GetTargetOffsetX", {{}, {Types::Number}}},
	{"GetTargetOffsetY", {{}, {Types::Number}}},
	{"SetTargetOffsetX", {{Types::Number}, {}}},
	{"SetTargetOffsetY", {{Types::Number}, {}}},
	{"GetUnitTargetOffsetX", {{Types::Int}, {Types::Int}}},
	{"GetUnitTargetOffsetY", {{Types::Int}, {Types::Int}}},
	{"SetUnitTargetOffsetX", {{Types::Int, Types::Int}, {}}},
	{"SetUnitTargetOffsetX", {{Types::Int, Types::Int}, {}}},
	#pragma endregion

	#pragma region Script Commands
	{"AddScriptToUnit", {{Types::Int, Types::String}, {}}},
	{"GetScriptVar", {{Types::Int, Types::String, Types::String}, {Types::unknown}}},
	{"SetScriptVar", {{Types::Int, Types::String, Types::String, Types::unknown}, {}}},
	#pragma endregion

	#pragma region Terrain and Wall Commands
	{"GetTerrain", {{Types::Number, Types::Number}, {Types::Number}}},
	{"SetTerrain", {{Types::Number, Types::Number, Types::Number}, {}}},
	#pragma endregion

	#pragma region Unit Commands
	{"Self", {{}, {Types::Number}}},
	{"GetUnitAttribute", {{Types::Number, Types::String}, {Types::Number}}},
	{"SetUnitAttribute", {{Types::Number, Types::String, Types::unknown}, {}}},
	{"CurrentCoords", {{}, {Types::Int, Types::Int}}},
	{"CurrentX", {{}, {Types::Number}}},
	{"CurrentY", {{}, {Types::Number}}},
	{"CurrentPixelCoords", {{}, {Types::Number, Types::Number}}},
	{"GetCoresWithVar", {{Types::String, Types::unknown}, {Types::List}}},
	{"Destroy", {{Types::Number, Types::Number},{}}},
	{"EnableTowerField", {{Types::Number, Types::Number, Types::Number, Types::Number, Types::Boolean},{}}},
	{"DisableTowerField", {{},{}}},
	{"CreateUnit", {{Types::String, Types::Number, Types::Number}, {Types::Number}}},
	#pragma endregion

	#pragma region Image Commands
	{"SetImage", {{Types::Int, Types::String, Types::String}, {}}},
	{"SetImagePosition", {{Types::Int, Types::String, Types::Number, Types::Number, Types::Number}, {}}},
	{"SetImagePositionX", {{Types::Int, Types::String, Types::Number}, {}}},
	{"SetImagePositionY", {{Types::Int, Types::String, Types::Number}, {}}},
	{"SetImagePositionZ", {{Types::Int, Types::String, Types::Number}, {}}},
	{"SetImageScale", {{Types::Int, Types::String, Types::Number, Types::Number}, {}}},
	{"SetImageScaleX", {{Types::Int, Types::String, Types::Number}, {}}},
	{"SetImageScaleY", {{Types::Int, Types::String, Types::Number}, {}}},
	{"SetImageRotation", {{Types::Int, Types::String, Types::Number}, {}}},
	{"SetImageColor", {{Types::Int, Types::String, Types::Number, Types::Number, Types::Number, Types::Number}, {}}},
	{"GetImagePosition", {{Types::Int, Types::String}, {Types::Number, Types::Number, Types::Number}}},
	{"GetImagePositionX", {{Types::Int, Types::String}, {Types::Number}}},
	{"GetImagePositionY", {{Types::Int, Types::String}, {Types::Number}}},
	{"GetImagePositionZ", {{Types::Int, Types::String}, {Types::Number}}},
	{"GetImageScale", {{Types::Int, Types::String}, {Types::Number, Types::Number}}},
	{"GetImageScaleX", {{Types::Int, Types::String, Types::unknown}, {Types::Number}}},
	{"GetImageScaleY", {{Types::Int, Types::String, Types::unknown}, {Types::Number}}},
	{"GetImageRotation", {{Types::Int, Types::String}, {Types::Number}}},
	{"GetImageColor", {{Types::Int, Types::String}, {Types::Int, Types::Int, Types::Int, Types::Int}}},
	{"RemoveImages", {{Types::Number}, {}}},
	#pragma endregion

	#pragma region Creeper Commands
	{"AddCreeper", {{Types::Int, Types::Int, Types::Number}, {}}},
	{"DamageCreeper", {{Types::Number, Types::Number, Types::Number, Types::Number, Types::Number, Types::Boolean}, {}}},
	{"SetCreeperNoLower", {{Types::Number, Types::Number, Types::Number}, {}}},
	{"IsCreeperInRange", {{Types::Number, Types::Number, Types::Number, Types::Number, Types::Boolean, Types::Boolean, Types::Boolean}, {Types::Boolean}}},
	{"SetCreeper", {{Types::Number, Types::Number, Types::Number}, {}}},
	{"GetCreeper", {{Types::Number, Types::Number}, {Types::Number}}},
	{"GetCreeperColors", {{}, {Types::Number, Types::Number, Types::Number, Types::Number, Types::Number, Types::Number}}},
	#pragma endregion

	#pragma region Utility Commands
	{"GetGameTimeFrames", {{}, {Types::Number}}},
	{"RandCoordsInRange", {{Types::Number, Types::Number, Types::Number}, {Types::Number, Types::Number}}},
	{"OperateWhilePaused", {{Types::Boolean}, {}}},
	#pragma endregion

	{"CreateSpore", {{Types::Number, Types::Number, Types::Number, Types::Number, Types::Number, Types::Number}, {}}},

	#pragma region Utility Commands
	{"SetPopupText", {{Types::String}, {}}},
	{"SetPopupTextAlwaysVisible", {{Types::Boolean}, {}}},
	{"SetPopupTextX", {{Types::Number}, {}}},
	{"SetPopupTextY", {{Types::Number}, {}}},
	{"SetText", {{Types::String}, {}}},
	{"SetTextAnchor", {{(Types)(Types::Int | Types::String)}, {}}},
	{"SetTextColor", {{Types::Number, Types::Number, Types::Number, Types::Number}, {}}},
	{"SetTextSize", {{Types::Number}, {}}},
	{"SetTextX", {{Types::Number}, {}}},
	{"SetTextY", {{Types::Number}, {}}},
	{"IsPaused", {{}, {Types::Boolean}}},
	#pragma endregion

	#pragma region Screen Commands
	{"ScreenHeight", {{}, {Types::Int}}},
	{"ScreenWidth", {{}, {Types::Int}}},
	{"SetScreenMode", {{Types::Boolean}, {}}},
	{"SetScreenCoords", {{Types::Number, Types::Number}, {}}},
	{"GetCameraPosition", {{}, {Types::Number, Types::Number}}},
	{"SetCameraPosition", {{Types::Number, Types::Number}, {}}},
	{"CellToPixel", {{Types::Number, Types::Number}, {Types::Number, Types::Number}}},
	#pragma endregion

	#pragma region Debugging
	{"Trace", {{Types::unknown}, {}}},
	{"Trace2", {{Types::unknown, Types::unknown}, {}}},
	{"Trace3", {{Types::unknown, Types::unknown, Types::unknown}, {}}},
	{"Trace4", {{Types::unknown, Types::unknown, Types::unknown, Types::unknown}, {}}},
	{"Trace5", {{Types::unknown, Types::unknown, Types::unknown, Types::unknown, Types::unknown}, {}}},
	{"TraceStack", {{}, {}}},
	{"ShowTraceLog", {{}, {}}},
	{"ClearTraceLog", {{}, {}}},
	{"HideTraceLog", {{}, {}}},
	#pragma endregion
};


static int CountReturn(Statement* node) {
	if(auto scope = dynamic_cast<Scope*>(node)) {
		int count = -1;
		for(auto i : scope->Body) {
			auto val = CountReturn(i);
			if(val == -1) continue;
			if(count == -1) {
				count = val;
				continue;
			}
			if(count != val) {
				Logger::Log(Error, i->Location, "Function always has to return the same amount of parameters");
			}
		}
		return count;
	}
	if(auto ret = dynamic_cast<ReturnStatement*>(node)) {
		return ret->ReturnValues.size();
	}
	if(auto loop = dynamic_cast<LoopStatement*>(node)) {
		return CountReturn(loop->Body);
	}
	if(auto ifs = dynamic_cast<IfStatement*>(node)) {
		int count = CountReturn(ifs->If);
		if(ifs->Else) {
			int v = CountReturn(ifs->Else);
			if(count == -1) {
				count = v;
			} else if(v != -1 && count != v) {
				Logger::Log(Error, ifs->Else->Location, "Function always has to return the same amount of parameters");
			}
		}
		return count;
	}

	return -1;
}

void Converter::newLine() {
	*buffer << '\n';
	for(int i = 0; i < depth; i++) {
		*buffer << '\t';
	}
}

const ScopeItem* Converter::findVar(const std::string& name) {
	for(int i = scopes.size() - 1; i >= 0; i--) {
		auto& scope = scopes[i];
		if(scope.variables.count(name)) {
			return &scope.variables[name];
		}
	}
	return nullptr;
}

const FunctionDecl* Converter::findFunc(const std::string& name) {
	for(int i = scopes.size() - 1; i >= 0; i--) {
		auto& scope = scopes[i];
		if(scope.functions.count(name)) {
			return scope.functions[name];
		}
	}
	return nullptr;
}

void Converter::declareFunctions(const std::vector<Statement*> body) {
	auto& scope = scopes.back();
	
	for(auto node : body) {
		if(auto func = dynamic_cast<FunctionDecl*>(node)) {
			// TODO: test duplicate names
			scope.functions[func->Name] = func;
		}
	}
}

void Converter::visit(AstNode* node, bool discard) {
	node->visit(*this, discard);
	if(discard) {
		assert(stack.empty());
	}
}

void Converter::visit(const ReturnStatement& node) {
	for(auto& value : node.ReturnValues) {
		visit(value, false);
	}

	// TODO: function stuff
	while(!stack.empty()) {
		stack.pop();
	}

	*buffer << "return ";
}

void Converter::visit(const LoopStatement& node) {
	// TODO: var declaration in Init
	if(node.Init) {
		visit(node.Init, true);
	}
	*buffer << "while ";
	visit(node.Condition, false);
	if(stack.size() != 1) {
		Logger::Log(Error, node.Location, "Stack error: too many parameters for loop condition");
	}
	stack.pop();
	*buffer << "repeat ";
	depth++;
	newLine();
	visit(node.Body, true);
	if(node.Repeat != nullptr) {
		newLine();
		visit(node.Repeat, true);
	}
	depth--;
	newLine();
	*buffer << "endwhile ";
}

void Converter::visit(const DoLoopStatement& node) {
	// TODO: implement
	Logger::Log(Error, node.Location, "Do loop not supported yet");
}

void Converter::visit(const ForOfStatement& node) {
	char listSymbol = 0;
	switch (forOfDepth) {
	case 0: listSymbol = 'I'; break;
	case 1: listSymbol = 'J'; break;
	case 2: listSymbol = 'K'; break;
		default:
			Logger::Log(Error, node.Location, "Maximum of 3 nested for..of loops allowed");
			break;
	}
	forOfDepth++;

	if(node.Name->Items.size() != 1 || node.Name->Items[0].Names.size() != 1) {
		Logger::Log(Error, node.Location, "Cannot have multi declaration in for of");
		return;
	}
	if(node.Name->Items[0].Value != nullptr) {
		Logger::Log(Error, node.Location, "Cannot have variable assignment in for of");
		return;
	}
	ScopeItem el;
	auto name = node.Name->Items[0].Names[0];
	if(findVar(name)) {
		Logger::Log(Warning, node.Name->Location, "Declaration of '" + name + "' hides previous local declaration");
		el.Codename = name + "_" + std::to_string(node.Location.line);
	} else {
		el.Codename = name;
	}
	el.Toketype = node.Name->TokenType;

	if(auto call = dynamic_cast<FunctionCall*>(node.Iterable)) {
		auto funcName = dynamic_cast<Variable*>(call->Left);
		if(!funcName) {
			Logger::Log(Error, node.Location, "Left side of function call has to be function name");
			return;
		}
		
		if(funcName->Name == "GetUnitsInRange" || 
		   funcName->Name == "GetEnemyUnitsInRange" ||
		   funcName->Name == "GetAllUnitsInRange" ||
		   funcName->Name == "GetCoresWithVar") {
			auto func = nativeVars[funcName->Name];
			*buffer << funcName->Name << '(';

			const size_t stackStart = stack.size();
			for(auto&& param : call->Params) {
				visit(param, false);
			}
			auto count = stack.size() - stackStart;
			if(count < func.consumes.size()) {
				Logger::Log(Error, funcName->Location, "Missing arguments for function \"" + funcName->Name + '"');
			} else if(count > func.consumes.size()) {
				Logger::Log(Error, funcName->Location, "Too many arguments for function \"" + funcName->Name + '"');
			} else {
				// Check of elements on stack are of correct type
				for(int i = func.consumes.size() - 1; i >= 0; i--) {
					const auto expected = func.consumes[i];
					const auto actual = stack.top();
					stack.pop();

					if(!(actual & expected)) {
						Logger::Log(Warning, node.Location, "parameter type might be wrong");
					}
				}
			}

			scopes.back().variables[name] = el;

			*buffer << ") 0 do";
			depth++;
			newLine();
			*buffer << "->" << el.Codename;
			newLine();
			visit(node.Body, true);
			depth--;
			newLine();
			*buffer << "loop";
			#ifdef deleteVars
			newLine();
			*buffer << "--" << el.Codename;
			#endif

			scopes.back().variables.erase(name);

			forOfDepth--;
			return;
		}
	}
	
	// assume node.Iterable is list
	auto stackStart = stack.size();
	visit(node.Iterable, false);
	auto count = stack.size() - stackStart;
	if(count == 0) {
		Logger::Log(Error, node.Iterable->Location, "Missing arguments for forOf loop");
	} else if(count > 1) {
		Logger::Log(Error, node.Iterable->Location, "Too many arguments for forOf loop");
		while (count > 0) {
			stack.pop();
			count--;
		}
	} else {
		auto t = stack.top();
		stack.pop();
		if((t & Types::List) == 0) {
			Logger::Log(Warning, node.Iterable->Location, "Right side of forOf loop has to be iterable");
		}
	}

	
	scopes.back().variables[name] = el;

	*buffer << "dup GetListCount 0 do";
	depth++;
	newLine();
	*buffer << "dup " << listSymbol << " GetListElement ->" << el.Codename;
	newLine();
	visit(node.Body, true);
	depth--;
	newLine();
	*buffer << "loop";
	#ifdef deleteVars
	newLine();
	*buffer << "--" << el.Codename;
	#endif

	scopes.back().variables.erase(name);

	forOfDepth--;
}

void Converter::visit(const IfStatement& node) {
	*buffer << "if(";
	visit(node.Condition, false);

	if(stack.empty()) {
		Logger::Log(Error, node.Location, "Missing parameter for if condition");
		stack.push(Types::unknown);
	} else if(stack.size() > 1) {
		Logger::Log(Error, node.Location, "Too many parameters for if condition");
		while (stack.size() > 1) { stack.pop(); }
	}
	*buffer << ")";
	stack.pop();

	depth++;
	newLine();
	visit(node.If, true);
	if(node.Else) {
		depth--;
		newLine();
		*buffer << "else";
		depth++;
		newLine();
		visit(node.Else, true);
	}
	depth--;
	newLine();
	*buffer << "endif ";
}

void Converter::visit(const Scope& node) {
	scopes.emplace_back();

	declareFunctions(node.Body);
	for (int i = 0; i < node.Body.size(); i++) {
		if(i > 0) {
			newLine();
		}
		visit(node.Body[i], true);
	}

	#ifdef deleteVars
	auto& scope = scopes.back();
	for(auto&& n : scope.variables) {
		if(n.second.Toketype == Tokentype::Let) {
			newLine();
			*buffer << "--" << n.second.Codename;
		}
	}
	#endif
	scopes.pop_back();
}

void Converter::visit(const LetStatement& node) {
	if(node.TokenType == Tokentype::Const) {
		Logger::Log(Error, node.Location, "Input variable only allowed at root level");
	}
	if(node.TokenType == Tokentype::Var) {
		Logger::Log(Warning, node.Location, "Prefer declaring global variables with let on root level");
	}

	auto& scope = scopes.back();

	for(const LetItem& item : node.Items) {
		if(item.Value) {
			visit(item.Value, false);

			if(stack.size() != item.Names.size()) {
				Logger::Log(Error, node.Location, "Right side of assign returns wrong amount of parameters");
				while(!stack.empty()) {
					stack.pop();
				}
			}
		}

		for(int i = item.Names.size() - 1; i >= 0; i--) {
			const auto& name = item.Names[i];
			ScopeItem el;
			if(scope.variables.count(name)) {
				Logger::Log(Error, item.Location, "Local variable " + name + " has already been declared");
				continue;
			}
			if(findVar(name)) {
				Logger::Log(Warning, item.Location, "Declaration of '" + name + "' hides previous local declaration");
				el.Codename = name + "_" + std::to_string(item.Location.line) + "_" + std::to_string(item.Location.column);
			} else {
				el.Codename = name;
			}
			el.Toketype = node.TokenType;
			if(item.Value) {
				*buffer << "->" + el.Codename + " ";
				stack.pop();
			}

			if(node.TokenType == Tokentype::Var) {
				scopes[0].variables[name] = el;
			}
			scope.variables[name] = el;
		}
	}
}

void Converter::visit(FunctionDecl& node) {
	auto retCount = CountReturn(node.Body);
	for(int i = 0; i < retCount; i++) {
		node.ReturnTypes.push_back(Types::unknown);
	}
	
	std::ostringstream temp;

	auto oldBuffer = buffer;
	auto oldDepth = depth;

	buffer = &temp;

	depth = 0;
	newLine();

	*buffer << ':' << node.Name;
	depth = 1;
	newLine();

	// done in declareFunctions
	// scopes.back().functions[node.Name] = &node;

	scopes.emplace_back();
	auto& scope = scopes.back();

	int i = 10;

	for(int i = node.Params.size() - 1; i >= 0; i--) {
		auto& param = node.Params[i];

		ScopeItem el;
		if(scope.variables.count(param.Name)) {
			Logger::Log(Error, node.Location, "Duplicate parameter name " + param.Name);
		} else if(findVar(param.Name)) {
			Logger::Log(Warning, node.Location, "Declaration of '" + param.Name + "' hides previous local declaration");
			el.Codename = param.Name + "_" + std::to_string(node.Location.line) + "_" + std::to_string(node.Location.column);
		} else {
			el.Codename = param.Name;
		}
		*buffer << "->" + el.Codename + " ";
		newLine();

		el.Toketype = Tokentype::Let;
		scope.variables[el.Codename] = el;
	}

	declareFunctions(node.Body->Body);

	for(int i = 0; i < node.Body->Body.size(); ++i) {
		auto el = node.Body->Body[i];
		if(i > 0) {
			newLine();
		}
		visit(el, true);
	}

	#ifdef deleteVars
	for(auto& n : scope.variables) {
		if(n.second.Toketype == Tokentype::Let) {
			newLine();
			*buffer << "--" << n.second.Codename;
		}
	}
	#endif
	scopes.pop_back();

	functionBuffer << temp.str();
	buffer = oldBuffer;
	depth = oldDepth;
}

void Converter::visit(const SelectExpression& node, bool discard) {
	const size_t startStack = stack.size();
	visit(node.Condition, false);
	auto count = stack.size() - startStack;
	if(count == 0) {
		Logger::Log(Error, node.Location, "Condition of select expression returns no arguments");
		stack.push(Types::unknown);
	} else if(count > 1) {
		Logger::Log(Error, node.Location, "Condition of select expression returns to many arguments");
		while(count > 1) {
			stack.pop();
			count--;
		}
	}
	auto val = stack.top();
	stack.pop();
	if(val & Types::Boolean == 0) {
		Logger::Log(Warning, node.Location, "Select condition has to return boolean or number");
	}

	*buffer << "if ";

	// TODO: compare types?
	visit(node.True, discard);
	const size_t lCount = stack.size() - startStack;

	*buffer << "else ";
	while(stack.size() > startStack) {
		stack.pop();
	}
	visit(node.False, discard);
	const size_t rCount = stack.size() - startStack;
	if(lCount != rCount) {
		Logger::Log(Error, node.Location, "Stack error: left and right side of ternary expression don't return the same amount of arguments");
	}
	// just keep the results of the false path for now
	*buffer << "endif ";
	
}

void Converter::visit(const PreExpression& node, bool discard) {
	if(!discard && node.Type == Tokentype::Sub && (dynamic_cast<IntLit*>(node.Right) || dynamic_cast<FloatLit*>(node.Right))) {
		*buffer << '-';
		visit(node.Right, discard);
		return;
	}
	auto start = stack.size();
	visit(node.Right, discard);
	auto diff = stack.size() - start;
	if(diff == 0) {
		Logger::Log(Error, node.Right->Location, "Right side of prefix expression returns no arguments");
		stack.push(Types::unknown);
	} else if(diff > 1) {
		Logger::Log(Error, node.Right->Location, "Right side of prefix expression returns to many arguments");
		while (diff > 1) {
			stack.pop();
			diff--;
		}
	}
	auto type = stack.top();
	stack.pop();
	if(type & Types::Number == 0) {
		Logger::Log(Warning, node.Location, "Right side of prefix expression should be number");
	}

	if(discard) {
		return;
	}

	switch(node.Type) {
		case Tokentype::Inc:
		case Tokentype::Dec:
			Logger::Log(Error, node.Location, "++ and -- Currently not supported");
			break;
		case Tokentype::Add: break;
		case Tokentype::Sub: *buffer << "neg "; break;
		case Tokentype::LNot: *buffer << "not "; break;
		case Tokentype::Not: *buffer << "neg 1 sub "; break;
		default: throw std::logic_error("Not reachable");
	}
	stack.push(Types::Number);
}

void Converter::visit(const PostExpression& node, bool discard) {
	if(auto var = dynamic_cast<Variable*>(node.Left)) {
		auto v = findVar(var->Name);
		visit(*var, false);

		auto type = stack.top();
		stack.pop();
		if(type & Types::Number == 0) {
			Logger::Log(Warning, node.Location, "Left side of postfix expression should be number");
		}

		if(v) {
			switch(node.Type) {
				case Tokentype::Inc: *buffer << "1 add "; break;
				case Tokentype::Dec: *buffer << "1 sub "; break;
				default: throw std::logic_error("Not reachable");
			}
			if(!discard) {
				*buffer << "dup ";
			}
			*buffer << "->" << v->Codename << ' ';
		}
	} else {
		Logger::Log(Error, node.Location, "left side of post expression has to be variable");
	}

	if(!discard) {
		stack.push(Types::Number);
	}
}

void Converter::visit(const Operation& node, bool discard) {
	if(node.Type == Tokentype::Assign) {
		if(auto var = dynamic_cast<Variable*>(node.Left)) {
			auto startStack = stack.size();
			visit(node.Right, false);
			if(stack.size() - startStack == 0) {
				Logger::Log(Error, node.Location, "Right side of assign returns nothing");
				stack.push(Types::unknown);
			} else if(stack.size() - startStack > 1) {
				Logger::Log(Error, node.Location, "Right side of assign returns to many parameters");
				while (stack.size() - startStack > 1) { stack.pop(); }
			}

			auto v = findVar(var->Name);
			if(v) {
				if(!discard) {
					*buffer << "dup ";
					stack.push(stack.top());
				}
				*buffer << "->" << v->Codename << " ";
			} else {
				Logger::Log(Error, node.Location, "Undefined variable " + var->Name);
			}
			stack.pop();
			return;
		}
		if(auto arr = dynamic_cast<Operation*>(node.Left)) {
			if(arr->Type == Tokentype::LBracket) {
				if(auto l = dynamic_cast<Variable*>(arr->Left)) {
					if(discard) {
						visit(l, false);          // Push list ref
						visit(arr->Right, false); // push index
						visit(node.Right, false); // push value
					} else {
						visit(node.Right, false); // push value
						*buffer << "dup ";
						visit(l, false); // Push list ref
						*buffer << "swap ";
						visit(arr->Right, false); // push index
						*buffer << "swap ";

						stack.push(Types::unknown);
					}

					*buffer << "SetListElement ";
					stack.pop(); // List
					stack.pop(); // index
					stack.pop(); // value

					return;
				} else {
					Logger::Log(Error, node.Left->Location, "Left side of array select has to be Variable");
					return;
				}
			}
		}
		if(auto asd = dynamic_cast<ArrayLit*>(node.Left)) {
			assert(discard);
			visit(node.Right, false);

			if(stack.size() != asd->Elements.size()) {
				Logger::Log(Error, node.Location, "Left side of multi assign has to have same amount of elements as right side");
			}

			for(int i = asd->Elements.size() - 1; i >= 0; i--) {
				auto var = dynamic_cast<Variable*>(asd->Elements[i]);
				if(var) {
					auto v = findVar(var->Name);
					if(v) {
						*buffer << "->" << v->Codename << " ";
					} else {
						Logger::Log(Error, node.Location, "Undefined variable " + var->Name);
					}
					stack.pop();
				} else {
					Logger::Log(Error, var->Location, "Left side of multi assign has to be variable");
				}
			}

			return;
		}
		Logger::Log(Error, node.Location, "Left side of an assign has to be a variable");
		return;
	}

	switch (node.Type) {
		case Tokentype::AddAssign:
		case Tokentype::SubAssign:
		case Tokentype::MulAssign:
		case Tokentype::DivAssign:
		case Tokentype::ModAssign:
		case Tokentype::AndAssign:
		case Tokentype::OrAssign:
		case Tokentype::XorAssign:
		case Tokentype::LshiftAssign:
		case Tokentype::RshiftAssign:
			Logger::Log(Error, node.Location, "Change assign not implemented");
			return;
	}

	if(discard) {
		auto start = stack.size();
		visit(node.Left, true);
		assert(stack.size() == start);
		visit(node.Right, true);
		assert(stack.size() == start);
		return;
	}

	auto visitLeft = [&]() {
		int stackStart = stack.size();
		visit(node.Left, false);
		if(stack.size() - stackStart == 0) {
			Logger::Log(Error, node.Left->Location, "left side of operation returns no arguments");
			stack.push(Types::unknown);
		} else if(stack.size() - stackStart > 1) {
			Logger::Log(Error, node.Left->Location, "left side of operation returns too many arguments");
			while (stack.size() - stackStart > 1) { stack.pop(); }
		}
	};

	auto visitRight = [&]() {
		int stackStart = stack.size();
		visit(node.Right, false);
		if(stack.size() - stackStart == 0) {
			Logger::Log(Error, node.Left->Location, "right side of operation returns no arguments");
			stack.push(Types::unknown);
		} else if(stack.size() - stackStart > 1) {
			Logger::Log(Error, node.Left->Location, "right side of operation returns too many arguments");
			while (stack.size() - stackStart > 1) { stack.pop(); }
		}
	};

	// eq0 neq0
	if(node.Type == Tokentype::Equals || node.Type == Tokentype::NEquals) {
		auto l = dynamic_cast<IntLit*>(node.Left);
		auto r = dynamic_cast<IntLit*>(node.Right);

		if(l != nullptr && l->Value == 0) {
			visitRight();
			*buffer << (node.Type == Tokentype::Equals ? "eq0 ": "neq0 ");
			stack.pop();
			stack.push(Types::Boolean);
			return;
		} if(r != nullptr && r->Value == 0) {
			visitLeft();
			*buffer << (node.Type == Tokentype::Equals ? "eq0 ": "neq0 ");
			stack.pop();
			stack.push(Types::Boolean);
			return;
		}
	}

	visitLeft();
	visitRight();

	switch(node.Type) {
		case Tokentype::LOr: *buffer << "or "; break;
		case Tokentype::LAnd: *buffer << "and "; break;
		case Tokentype::NullCoal: *buffer << "swap dup if swap pop else pop endif "; break;
		case Tokentype::Equals: *buffer << "eq "; break;
		case Tokentype::NEquals: *buffer << "neq ";  break;
		case Tokentype::Less: *buffer << "lt "; break;
		case Tokentype::LessEq: *buffer << "lte "; break;
		case Tokentype::Greater: *buffer << "gt "; break;
		case Tokentype::GreaterEq: *buffer << "gte "; break;
		case Tokentype::Or:
		case Tokentype::Xor:
		case Tokentype::And: throw std::logic_error("Not implemented");
		case Tokentype::LShift: *buffer << "2 swap pow mul "; break;
		case Tokentype::RShift: *buffer << "2 swap pow div "; break;
		case Tokentype::Sub: *buffer << "sub "; break;
		case Tokentype::Mul: *buffer << "mul "; break;
		case Tokentype::Pow: *buffer << "pow "; break;
		case Tokentype::Div: *buffer << "div "; break;
		case Tokentype::Mod: *buffer << "mod "; break;
		case Tokentype::Add: {
			auto l = stack.top();
			stack.pop();
			auto r = stack.top();
			stack.pop();
			if((l != unknown && l & Types::String) || (r != unknown && r & Types::String)) {
				*buffer << "concat ";
				Logger::Log(Warning, node.Location, "Use template strings instead of addition");
				stack.push(Types::String);
			} else {
				*buffer << "add ";
				stack.push(Types::Number);
			}
			return;
		}
		case Tokentype::LBracket:
			if(!dynamic_cast<Variable*>(node.Left)) {
				Logger::Log(Error, node.Left->Location, "Left side of array select has to be Variable");
			}
			*buffer << "GetListElement ";
			if(stack.top() & Types::List == 0) {
				Logger::Log(Warning, node.Left->Location, "Left side of array select has to be list");
			}
			stack.pop();
			if(stack.top() & Types::Int == 0) {
				Logger::Log(Warning, node.Right->Location, "Index of array select has to be integer");
			}
			stack.pop();
			stack.push(Types::unknown);
			return;
		default: throw std::logic_error("Not reachable");
	}
	if(stack.top() & Types::Int == 0) {
		Logger::Log(Warning, node.Left->Location, "Left side of operation has to be number");
	}
	stack.pop();
	if(stack.top() & Types::Int == 0) {
		Logger::Log(Warning, node.Left->Location, "Right side of operation has to be number");
	}
	stack.pop();
	stack.push(Types::Number);
}

void Converter::visit(const FunctionCall& node, bool discard) {
	auto var = dynamic_cast<Variable*>(node.Left);
	if(!var) {
		Logger::Log(Error, node.Location, "Left side of function call has to be function name");
		return;
	}

	auto func = findFunc(var->Name);
	if(func) { 
		*buffer << "@" << var->Name << '(';
	} else if(nativeVars.count(var->Name)) {
		*buffer << var->Name << '(';
	}

	const size_t stackStart = stack.size();
	for(auto&& param : node.Params) {
		visit(param, false);
	}
	size_t count = stack.size() - stackStart;
	*buffer << ") ";

	if(func) {
		if(count < func->Params.size()) {
			Logger::Log(Error, var->Location, "Missing arguments for function \"" + var->Name + '"');
		} else if(count > func->Params.size()) {
			Logger::Log(Error, var->Location, "Too many arguments for function \"" + var->Name + '"');
		}
		// TODO: better type checking
		while (count > 0) { 
			stack.pop(); 
			count--; 
		}

		if(discard && func->ReturnTypes.size() != 0) {
			Logger::Log(Warning, node.Location, "Discarding function result");
			*buffer << "ClearStack ";
		} else {
			for(int i = 0; i < func->ReturnTypes.size(); i++) {
				stack.push(func->ReturnTypes[i]);
			}
		}
	} else if(nativeVars.count(var->Name)) {
		auto& func = nativeVars[var->Name];
		if(count < func.consumes.size()) {
			Logger::Log(Error, var->Location, "Missing arguments for function \"" + var->Name + '"');
		} else if(count > func.consumes.size()) {
			Logger::Log(Error, var->Location, "Too many arguments for function \"" + var->Name + '"');
		} else {
			// Check of elements on stack are of correct type
			for(int i = func.consumes.size() - 1; i >= 0; i--) {
				const auto expected = func.consumes[i];
				const auto actual = stack.top();
				stack.pop();

				if(!(actual & expected)) {
					Logger::Log(Warning, node.Location, "parameter type might be wrong");
				}
			}
		}

		if(discard && !func.returns.empty()) {
			*buffer << "ClearStack ";
		} else {
			if(var->Name == "GetUnitsInRange" || 
			   var->Name == "GetEnemyUnitsInRange" || 
			   var->Name == "GetAllUnitsInRange" || 
			   var->Name == "GetCoresWithVar") {
				// fucked up shit because of unknown return count
				*buffer << "CreateList swap 0 do swap dup2 AppendToList pop loop ";
				stack.push(Types::List);
			} else {
				for(const auto t : func.returns) {
					stack.push(t);
				}
			}
		}
	} else {
		Logger::Log(Error, var->Location, "Unknown function \"" + var->Name + '"');
		while(stack.size() - stackStart > 0) { stack.pop(); }
	}
}

void Converter::visit(const IntLit& node, bool discard) {
	if(!discard) {
		*buffer << node.Value << " ";
		stack.push(Types::Int);
	}
}

void Converter::visit(const FloatLit& node, bool discard) {
	if(!discard) {
		*buffer << node.Value;
		if(fmod(node.Value, 1) == 0) {
			*buffer << ".0 ";
		} else {
			*buffer << " ";
		}
		stack.push(Types::Float);
	}
}

void Converter::visit(const StringLit& node, bool discard) {
	if(!discard) {
		*buffer << '\"' << node.Value << "\" ";
		stack.push(Types::String);
	}
}

void Converter::visit(const BoolLit& node, bool discard) {
	if(!discard) {
		*buffer << (node.Value ? "TRUE " : "FALSE ");
		stack.push(Types::Boolean);
	}
}

void Converter::visit(const ArrayLit& node, bool discard) {
	if(discard) {
		for(const auto element : node.Elements) {
			visit(element, true);
		}
	} else {
		const size_t startStack = stack.size();
		*buffer << "CreateList ";

		for(auto element : node.Elements) {
			*buffer << "dup ";
			visit(element, false);
			if(stack.size() - startStack == 0) {
				Logger::Log(Error, element->Location, "array element returns no elements");
			} else if(stack.size() - startStack > 1) {
				Logger::Log(Error, element->Location, "array element returns multiple elements");
			}
			*buffer << "AppendToList ";
			while(stack.size() > startStack) {
				stack.pop();
			}
		}

		stack.push(Types::List);
	}
}

void Converter::visit(const Variable& node, bool discard) {
	auto v = findVar(node.Name);
	if(v) {
		if(!discard) {
			*buffer << "<-" << v->Codename << ' ';
			// stack.push(v->Typename);
			stack.push(Types::unknown);
		}
	} else {
		if(nativeVars.count(node.Name)) {
			// TODO: if (function is const && discard) the call can be skipped
			auto var = nativeVars[node.Name];
			*buffer << node.Name << " ";
			if(!var.consumes.empty()) {
				Logger::Log(Error, node.Location, node.Name + " is a function and not a variable");
			}
			// Push all returns onto the stack
			for(const auto t : var.returns) {
				stack.push(t);
			}

			if(discard) {
				for(int i = 0; i < var.returns.size(); i++) {
					*buffer << "pop ";
					stack.pop();
				}
			}
		} else {
			Logger::Log(Error, node.Location, "Undefined variable " + node.Name);
			stack.push(Types::unknown);
		}
	}
}

std::string Converter::Convert(const std::vector<Statement*>& ast, const std::string& fileName) {
	std::ostringstream buf;
	buffer = &buf;

	FunctionDecl* main = nullptr;
	int count = 0;

	// script vars
	for(auto node : ast) {
		if(auto var = dynamic_cast<LetStatement*>(node)) {
			if(var->TokenType == Tokentype::Const) {
				
				for(LetItem& item : var->Items) {
					if(item.Names.size() != 1) {
						Logger::Log(Error, item.Location, "Cannot have multi declaration for input variables");
					}

					auto name = item.Names[0];
					*buffer << '$' << name << ':';

					// TODO: do better const eval
					if(dynamic_cast<IntLit*>(item.Value) || dynamic_cast<FloatLit*>(item.Value) || dynamic_cast<StringLit*>(item.Value) || dynamic_cast<BoolLit*>(item.Value)) {
						visit(item.Value, false);
						stack.pop();
					} else if(const auto op = dynamic_cast<PreExpression*>(item.Value)) {
						if(op->Type == Tokentype::Sub && (dynamic_cast<IntLit*>(op->Right) || dynamic_cast<FloatLit*>(op->Right))) {
							*buffer << "-";
							visit(op->Right, false);
							stack.pop();
						} else {
							Logger::Log(Error, var->Location, "Right site of input variable has to be simple constant");
						}
					} else {
						Logger::Log(Error, var->Location, "Right site of input variable has to be simple constant");
					}

					newLine();
					scopes[0].variables[name] = ScopeItem{ name, Tokentype::Const };
				}

				continue;
			}
		}
		if(auto func = dynamic_cast<FunctionDecl*>(node)) {
			if(func->Name == "main") {
				main = func;
			} else {
				scopes[0].functions[func->Name] = func;
			}
			continue;
		}
		count++;
	}

	*buffer << '\n';
	
	// once
	if(count != 0) {
		*buffer << "once";
		depth = 1;
		newLine();

		bool first = true;
		for(int i = 0; i < ast.size(); ++i) {
			auto node = ast[i];

			if(dynamic_cast<FunctionDecl*>(node)) {
				continue;
			}
			if(auto l = dynamic_cast<LetStatement*>(node)) {
				if(l->TokenType == Tokentype::Const) {
					continue;
				}
			}
			if(!first) {
				newLine();
			}
			first = false;
			visit(node, true);
		}
		depth = 0;
		newLine();
		*buffer << "endonce\n\n";
	}

	// main
	if(!main) {
		Logger::Log(Warning, fileName, "No main function found");
	} else {
		visit(main->Body, true);
	}

	// functions
	for(auto node : ast) {
		if(auto func = dynamic_cast<FunctionDecl*>(node)) {
			if(func->Name == "main") { continue; }
			depth = 0;
			visit(node, true);
		}
	}

	buf << "\n" << functionBuffer.str();
	return buf.str();
}
