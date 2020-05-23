#include "logger.h"
#include "converter.h"

#include <algorithm>
#include <map>
#include <set>
#include <sstream>
#include <cmath>
#include <cassert>

struct NativeVar {
	std::vector<Types> consumes;
	std::vector<Types> returns;
};

// TODO: eq0 neq0

std::map<std::string, NativeVar> nativeVars = {
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
	{"abs", {{Types::Number}, {Types::Number}}},
	{"acos", {{Types::Number}, {Types::Number}}},
	{"approximately", {{Types::Number, Types::Number}, {Types::Number}}},
	{"asfloat", {{Types::Number}, {Types::Float}}},
	{"asin", {{Types::Number}, {Types::Number}}},
	{"asint", {{Types::Number}, {Types::Number}}},
	{"atan", {{Types::Number}, {Types::Number}}},
	{"atan2", {{Types::Number, Types::Number}, {Types::Number}}},
	{"ceil", {{Types::Number}, {Types::Number}}},
	{"cos", {{Types::Number}, {Types::Number}}},
	{"E", {{}, {Types::Float}}},
	{"floor", {{Types::Number}, {Types::Number}}},
	{"ln", {{Types::Number}, {Types::Number}}},
	{"log", {{Types::Number, Types::Number}, {Types::Number}}},
	{"log10", {{Types::Number}, {Types::Number}}},
	{"max", {{Types::Number, Types::Number}, {Types::Number}}},
	{"min", {{Types::Number, Types::Number}, {Types::Number}}},
	{"PI", {{}, {Types::Float}}},
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

	// list commands

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
	#pragma endregion

	#pragma region Utility Commands
	{"GetGameTimeFrames", {{}, {Types::Number}}},
	#pragma endregion

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
	#pragma endregion

	#pragma region Screen Commands
	{"ScreenHeight", {{}, {Types::Int}}},
	{"ScreenWidth", {{}, {Types::Int}}},
	{"SetScreenMode", {{Types::Boolean}, {}}},
	{"SetScreenCoords", {{Types::Number, Types::Number}, {}}},
	{"GetCameraPosition", {{}, {Types::Number, Types::Number}}},
	{"SetCameraPosition", {{Types::Number, Types::Number}, {}}},
	#pragma endregion

	#pragma region Debugging
	{"Trace", {{Types::unknown}, {}}},
	{"ShowTraceLog", {{}, {}}},
	#pragma endregion
};

void Converter::newLine() {
	buffer << '\n';
	for(int i = 0; i < depth; i++) {
		buffer << '\t';
	}
}

const ScopeItem* Converter::findVar(const std::string& name) {
	for(int i = scopes.size() - 1; i >= 0; i--) {
		auto& scope = scopes[i];
		if(scope.count(name)) {
			return &scope[name];
		}
	}
	return nullptr;
}

void Converter::visit(AstNode* node, bool discard) {
	node->visit(*this, discard);
}

void Converter::visit(const ReturnStatement& node) {
	assert(stack.empty());

	for(auto& value : node.ReturnValues) {
		visit(value, false);
	}

	while(!stack.empty()) {
		stack.pop();
	}

	buffer << "return ";
}

void Converter::visit(const LoopStatement& node) {
	// TODO: var declaration in Init
	if(node.Init) {
		visit(node.Init, true);
	}
	buffer << "while ";
	visit(node.Condition, false);
	if(stack.size() != 1) {
		Logger::Log(Error, node.Location, "Stack error: too many parameters for loop condition");
	}
	buffer << "repeat ";
	visit(node.Body, true);
	visit(node.Repeat, true);
	buffer << "endwhile";
	assert(stack.empty());
}

void Converter::visit(const DoLoopStatement& node) {
	Logger::Log(Error, node.Location, "Not implemented");
}

void Converter::visit(const IfStatement& node) {
	assert(stack.empty());

	buffer << "if(";
	visit(node.Condition, false);
	if(stack.empty()) {
		Logger::Log(Error, node.Location, "Missing parameter for if condition");
	} else if(stack.size() > 1) {
		Logger::Log(Error, node.Location, "Too many parameters for if condition");
	}
	buffer << ")";
	stack.pop();

	depth++;
	newLine();
	visit(node.If, true);
	assert(stack.empty());
	if(node.Else) {
		depth--;
		newLine();
		buffer << "else";
		depth++;
		newLine();
		visit(node.Else, true);
		assert(stack.empty());
	}
	depth--;
	newLine();
	buffer << "endif ";
}

void Converter::visit(const Scope& node) {
	scopes.emplace_back();
	for(auto&& i : node.Body) {
		visit(i, true);
		newLine();
		assert(stack.empty());
	}
	auto scope = scopes.back();
	scopes.pop_back();
	for(auto&& n : scope) {
		if(n.second.Toketype == Tokentype::Let) {
			buffer << "--" << n.second.Codename;
			newLine();
		}
	}
}

void Converter::visit(LetStatement& node) {
	if(node.TokenType == Tokentype::Const) {
		Logger::Log(Error, node.Location, "Input variable only allowed at root level");
	}
	if(node.TokenType == Tokentype::Var) {
		Logger::Log(Warning, node.Location, "Prefer declaring global variables with let on root level");
	}

	auto& scope = scopes.back();

	for(LetItem& item : node.Items) {
		if(item.Value) {
			visit(item.Value, false);

			if(stack.size() != item.Names.size()) {
				Logger::Log(Error, node.Location, "Right side of assign returns wrong amount of parameters");
				while(!stack.empty()) {
					stack.pop();
				}
			} else {
				for(int i = item.Names.size() - 1; i >= 0; i--) {
					const auto& name = item.Names[i];
					ScopeItem el;
					if(scope.count(name)) {
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
					el.Typename = stack.top();

					stack.pop();
					buffer << "->" + el.Codename + " ";

					if(node.TokenType == Tokentype::Var) {
						scopes[0][name] = el;
					}
					scope[name] = el;
				}
			}
		} else {
			for(int i = item.Names.size() - 1; i >= 0; i--) {
				const auto& name = item.Names[i];
				ScopeItem el;
				if(scope.count(name)) {
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
				el.Typename = Types::unknown;

				if(node.TokenType == Tokentype::Var) {
					scopes[0][name] = el;
				}
				scope[name] = el;
			}
		}
	}
}

void Converter::visit(const FunctionDecl& node) {
	Logger::Log(Error, node.Location, "Nested functions not allowed");
}

void Converter::visit(const SelectExpression& node, bool discard) {
	const size_t startStack = stack.size();
	visit(node.Condition, false);
	if(stack.size() - startStack == 0) {
		Logger::Log(Error, node.Location, "");
	} else if(stack.size() - startStack > 1) {
		Logger::Log(Error, node.Location, "");
	}
	buffer << "if ";
	while(stack.size() > startStack) {
		stack.pop();
	}

	visit(node.True, discard);
	const size_t lCount = stack.size() - startStack;

	buffer << "else ";
	while(stack.size() > startStack) {
		stack.pop();
	}
	visit(node.False, discard);
	const size_t rCount = stack.size() - startStack;
	if(lCount != rCount) {
		Logger::Log(Error, node.Location, "Stack error: left and right side of ternary expression don't return the same amount of arguments");
	}
	buffer << "endif ";
	if(discard) {
		assert(stack.empty());
	}
}

void Converter::visit(const PreExpression& node, bool discard) {
	if(!discard && node.Type == Tokentype::Sub && (dynamic_cast<IntLit*>(node.Right) || dynamic_cast<FloatLit*>(node.Right))) {
		buffer << '-';
		visit(node.Right, discard);
		return;
	}
	visit(node.Right, discard);
	if(discard) {
		assert(stack.empty());
		return;
	}

	switch(node.Type) {
		case Tokentype::Inc:
		case Tokentype::Dec:
			Logger::Log(Error, node.Location, "++ and -- Currently not supported");
			break;
		case Tokentype::Add: break;
		case Tokentype::Sub: buffer << "neg ";
			break;
		case Tokentype::LNot: buffer << "not ";
			break;
		case Tokentype::Not: buffer << "neg 1 sub ";
			break;
		default: throw std::logic_error("Not reachable");
	}
}

void Converter::visit(const PostExpression& node, bool discard) {
	if(auto var = dynamic_cast<Variable*>(node.Left)) {
		auto v = findVar(var->Name);

		visit(*var, false);
		if(v) {
			switch(node.Type) {
				case Tokentype::Inc: buffer << "1 add ";
					break;
				case Tokentype::Dec: buffer << "1 sub ";
					break;
				default: throw std::logic_error("Not reachable");
			}
			if(!discard) {
				buffer << "dup ";
				stack.push(stack.top());
			}
			buffer << "->" << v->Codename;
			stack.pop();
		}
	} else {
		Logger::Log(Error, node.Location, "left side of post expression has to be variable");
	}
}

void Converter::visit(const Operation& node, bool discard) {
	if(node.Type == Tokentype::Assign) {
		if(auto var = dynamic_cast<Variable*>(node.Left)) {
			auto startStack = stack.size();
			visit(node.Right, false);
			if(stack.size() - startStack == 0) {
				Logger::Log(Error, node.Location, "Right side of assign returns nothing");
			} else if(stack.size() - startStack > 1) {
				Logger::Log(Error, node.Location, "Right side of assign returns to many parameters");
			}

			auto v = findVar(var->Name);
			if(v) {
				if(!discard) {
					buffer << "dup ";
					stack.push(stack.top());
				}
				buffer << "->" << v->Codename << " ";
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
						buffer << "dup ";
						visit(l, false); // Push list ref
						buffer << "swap ";
						visit(arr->Right, false); // push index
						buffer << "swap ";

						stack.push(Types::unknown);
					}

					buffer << "InsertListElement ";
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
						buffer << "->" << v->Codename << " ";
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

	if(discard) {
		visit(node.Left, true);
		assert(stack.empty());
		visit(node.Right, true);
		assert(stack.empty());
		return;
	}

	int stackStart = stack.size();
	visit(node.Left, false);
	if(stack.size() - stackStart == 0) {
		Logger::Log(Error, node.Left->Location, "left side of operation returns no arguments");
	} else if(stack.size() - stackStart > 1) {
		Logger::Log(Error, node.Left->Location, "left side of operation returns too many arguments");
	}

	stackStart = stack.size();
	visit(node.Right, false);
	if(stack.size() - stackStart == 0) {
		Logger::Log(Error, node.Left->Location, "right side of operation returns no arguments");
	} else if(stack.size() - stackStart > 1) {
		Logger::Log(Error, node.Left->Location, "right side of operation returns too many arguments");
	}

	switch(node.Type) {
		case Tokentype::LOr: buffer << "or ";
			break;
		case Tokentype::LAnd: buffer << "and ";
			break;
		case Tokentype::NullCoal: buffer << "swap dup if swap pop else pop endif ";
			break;
		case Tokentype::Equals: buffer << "eq ";
			break;
		case Tokentype::NEquals: buffer << "neq ";
			break;
		case Tokentype::Less: buffer << "lt ";
			break;
		case Tokentype::LessEq: buffer << "lte ";
			break;
		case Tokentype::Greater: buffer << "gt ";
			break;
		case Tokentype::GreaterEq: buffer << "gte ";
			break;
		case Tokentype::Or:
		case Tokentype::Xor:
		case Tokentype::And: throw std::logic_error("Not implemented");
		case Tokentype::LShift: buffer << "2 swap pow mul ";
		case Tokentype::RShift: buffer << "2 swap pow div ";
		case Tokentype::Sub: buffer << "sub ";
			break;
		case Tokentype::Mul: buffer << "mul ";
			break;
		case Tokentype::Pow: buffer << "pow ";
			break;
		case Tokentype::Div: buffer << "div ";
			break;
		case Tokentype::Mod: buffer << "mod ";
			break;
		case Tokentype::Add:
		{
			auto l = stack.top();
			stack.pop();
			auto r = stack.top();
			stack.pop();
			if((l != unknown && l & Types::String) || (r != unknown && r & Types::String)) {
				buffer << "concat ";
				Logger::Log(Warning, node.Location, "Use template strings instead of addition");
				stack.pop();
				stack.pop();
				stack.push(Types::String);
			} else {
				buffer << "add ";
				stack.push(Types::Number);
			}
			return;
		}
		case Tokentype::LBracket:
			if(!dynamic_cast<Variable*>(node.Left)) {
				Logger::Log(Error, node.Left->Location, "Left side of array select has to be Variable");
			}
			buffer << "GetListElement ";
			break;
		default:
			throw std::runtime_error("Not reachable");
	}
	stack.pop();
}

void Converter::visit(const FunctionCall& node, bool discard) {
	auto var = dynamic_cast<Variable*>(node.Left);
	if(!var) {
		Logger::Log(Error, node.Location, "Left side of function call has to be function name");
		return;
	}


	if(functions.count(var->Name)) {
		buffer << "@" << var->Name << '(';
	} else if(nativeVars.count(var->Name)) {
		buffer << var->Name << '(';
	} else { }

	const size_t stackStart = stack.size();
	for(auto&& param : node.Params) {
		visit(param, false);
	}
	const size_t count = stack.size() - stackStart;
	buffer << ')';

	if(functions.count(var->Name)) {
		auto& func = *functions[var->Name];

		if(count < func.Params.size()) {
			Logger::Log(Error, var->Location, "Missing arguments for function \"" + var->Name + '"');
		} else if(count > func.Params.size()) {
			Logger::Log(Error, var->Location, "Too many arguments for function \"" + var->Name + '"');
		}
		for(int i = 0; i < count; i++) {
			stack.pop();
		}

		if(discard && func.ReturnCount != 0) {
			buffer << "ClearStack ";
		} else {
			// TODO: better type checking
			for(int i = 0; i < func.ReturnCount; i++) {
				stack.push(Types::unknown);
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
			buffer << "ClearStack ";
		} else {
			for(const auto t : func.returns) {
				stack.push(t);
			}
		}
	} else {
		Logger::Log(Error, var->Location, "Unknown function \"" + var->Name + '"');
	}
}

void Converter::visit(const IntLit& node, bool discard) {
	if(!discard) {
		buffer << node.Value << " ";
		stack.push(Types::Int);
	}
}

void Converter::visit(const FloatLit& node, bool discard) {
	if(!discard) {
		buffer << node.Value;
		if(fmod(node.Value, 1) == 0) {
			buffer << ".0 ";
		} else {
			buffer << " ";
		}
		stack.push(Types::Float);
	}
}

void Converter::visit(const StringLit& node, bool discard) {
	if(!discard) {
		buffer << '\"' << node.Value << "\" ";
		stack.push(Types::String);
	}
}

void Converter::visit(const BoolLit& node, bool discard) {
	if(!discard) {
		buffer << (node.Value ? "TRUE " : "FALSE ");
		stack.push(Types::Boolean);
	}
}

void Converter::visit(const ArrayLit& node, bool discard) {
	if(discard) {
		for(const auto element : node.Elements) {
			visit(element, true);
			assert(stack.empty());
		}
	} else {
		const size_t startStack = stack.size();
		buffer << "CreateList ";

		for(auto element : node.Elements) {
			buffer << "dup ";
			visit(element, false);
			if(stack.size() - startStack == 0) {
				Logger::Log(Warning, element->Location, "array element returns no elements");
			} else if(stack.size() - startStack > 1) {
				Logger::Log(Warning, element->Location, "array element returns multiple elements");
			}
			while(stack.size() > startStack) {
				buffer << "AppendToList ";
				stack.pop();
			}
		}

		stack.push(Types::List);
	}
}

void Converter::visit(const Variable& node, bool discard) {
	auto v = findVar(node.Name);
	if(v) {
		buffer << "<-" << v->Codename << ' ';
		stack.push(v->Typename);
	} else {
		if(nativeVars.count(node.Name)) {
			// TODO: if (function is const && discard) the call can be skipped
			auto var = nativeVars[node.Name];
			buffer << node.Name << " ";
			if(!var.consumes.empty()) {
				Logger::Log(Error, node.Location, node.Name + " is a function and not a variable");
			}
			// Push all returns onto the stack
			for(const auto t : var.returns) {
				stack.push(t);
			}

			if(discard && !stack.empty()) {
				buffer << "ClearStack ";
			}
		} else {
			Logger::Log(Error, node.Location, "Undefined variable " + node.Name);
		}
	}
}

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

void Converter::visitFunctionDecl(const FunctionDecl& node) {
	depth = 0;
	newLine();

	depth = 1;
	buffer << ':' << node.Name;
	newLine();

	scopes.emplace_back();
	auto& scope = scopes.back();

	for(int i = node.Params.size() - 1; i >= 0; i--) {
		auto& param = node.Params[i];

		if(scope.count(param.Name)) {
			Logger::Log(Error, node.Location, "Local variable " + param.Name + " has already been declared");
			continue;
		}
		ScopeItem el;
		if(findVar(param.Name)) {
			Logger::Log(Warning, node.Location, "Declaration of '" + param.Name + "' hides previous local declaration");
			el.Codename = param.Name + "_" + std::to_string(node.Location.line) + "_" + std::to_string(node.Location.column);
		} else {
			el.Codename = param.Name;
		}
		buffer << "->" + el.Codename + " ";
		newLine();

		el.Toketype = Tokentype::Let;
		el.Typename = Types::unknown;
		scope[el.Codename] = el;
	}

	for(auto&& i : node.Body->Body) {
		visit(i, true);
		newLine();
		assert(stack.empty());
	}

	for(auto& n : scope) {
		if(n.second.Toketype == Tokentype::Let) {
			buffer << "--" << n.second.Codename;
			newLine();
		}
	}
	scopes.pop_back();
}

std::string crpl(const std::vector<AstNode*>& ast, const std::string& fileName) {
	Converter cnvrt;
	FunctionDecl* main = nullptr;
	int count = 0;

	for(auto node : ast) {
		if(auto func = dynamic_cast<FunctionDecl*>(node)) {
			if(func->Name == "main") {
				main = func;
				continue;
			}
			auto count = CountReturn(func->Body);
			if(count == -1) {
				count = 0;
			}
			func->ReturnCount = count;

			if(nativeVars.count(func->Name)) {
				Logger::Log(Error, node->Location, "Native function with same name already exists");
			}
			if(cnvrt.functions.count(func->Name)) {
				Logger::Log(Error, node->Location, "function \"" + func->Name + "\" exists multiple times");
			}
			cnvrt.functions[func->Name] = func;
			continue;
		}
		if(auto var = dynamic_cast<LetStatement*>(node)) {
			if(var->TokenType == Tokentype::Const) {
				for(LetItem& item : var->Items) {
					if(item.Names.size() != 1) {
						Logger::Log(Error, item.Location, "Cannot have multi declaration for input variables");
					}


					auto name = item.Names[0];
					cnvrt.buffer << '$' << name << ':';

					// TODO: do better const eval
					if(dynamic_cast<IntLit*>(item.Value) || dynamic_cast<FloatLit*>(item.Value) || dynamic_cast<StringLit*>(item.Value) || dynamic_cast<BoolLit*>(item.Value)) {
						cnvrt.visit(item.Value, false);
					} else if(const auto op = dynamic_cast<PreExpression*>(item.Value)) {
						if(op->Type == Tokentype::Sub && (dynamic_cast<IntLit*>(op->Right) || dynamic_cast<FloatLit*>(op->Right))) {
							cnvrt.buffer << "-";
							cnvrt.visit(op->Right, false);
						} else {
							Logger::Log(Error, var->Location, "Right site of input variable has to be simple constant");
							cnvrt.stack.push(Types::unknown);
						}
					} else {
						Logger::Log(Error, var->Location, "Right site of input variable has to be simple constant");
						cnvrt.stack.push(Types::unknown);
					}

					cnvrt.newLine();
					cnvrt.scopes[0][name] = ScopeItem{ name, cnvrt.stack.top(), Tokentype::Const };
					cnvrt.stack.pop();
				}
			}
		}
		count++;
	}

	if(!main) {
		Logger::Log(Error, fileName, "No main function found");
	}

	if(count != 0) {
		cnvrt.buffer << "once";
		cnvrt.depth = 1;
		cnvrt.newLine();

		for(auto node : ast) {
			if(dynamic_cast<FunctionDecl*>(node)) {
				continue;
			}
			if(auto l = dynamic_cast<LetStatement*>(node)) {
				if(l->TokenType == Tokentype::Const) {
					continue;
				}
			}
			cnvrt.visit(node, true);
		}
		cnvrt.depth = 0;
		cnvrt.newLine();
		cnvrt.buffer << "endonce\n\n";
	}

	cnvrt.visit(main->Body, true);

	for(auto node : ast) {
		auto func = dynamic_cast<FunctionDecl*>(node);
		if(!func || func->Name == "main") {
			continue;
		}
		cnvrt.visitFunctionDecl(*func);
	}

	return cnvrt.buffer.str();
}
