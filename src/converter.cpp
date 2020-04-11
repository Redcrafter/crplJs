#include "logger.h"
#include "converter.h"

#include <map>
#include <set>
#include <sstream>
#include <cmath>
#include <cassert>

struct NativeVar {
	int consumes;
	int returns;
};

std::map<std::string, NativeVar> nativeVars = {
	// "I", "J", "K"
	{"CurrentCoords", {0, 2}},
	{"CONST_ACPACKETREQUESTDELAY", {0, 1}},
	{"CONST_AMMO", {0, 1}},
	{"CONST_AMMOAC", {0, 1}},
	{"CONST_BEAMTARGET", {0, 1}},
	{"CONST_BUILDCOST", {0, 1}},
	{"CONST_CANREQUESTAMMO", {0, 1}},
	{"CONST_CELLHEIGHT", {0, 1}},
	{"CONST_CELLWIDTH", {0, 1}},
	{"CONST_CONNECTABLE", {0, 1}},
	{"CONST_COORDX", {0, 1}},
	{"CONST_COORDY", {0, 1}},
	{"CONST_COUNTSFORVICTORY", {0, 1}},
	{"CONST_CREATEPZ", {0, 1}},
	{"CONST_DESTROYMODE", {0, 1}},
	{"CONST_DESTROYONDAMAGE", {0, 1}},
	{"CONST_HEALRATE", {0, 1}},
	{"CONST_HEALTH", {0, 1}},
	{"CONST_ISBUILDING", {0, 1}},
	{"CONST_ISDESTROYED", {0, 1}},
	{"CONST_ISLANDED", {0, 1}},
	{"CONST_MAXAMMO", {0, 1}},
	{"CONST_MAXAMMOAC", {0, 1}},
	{"CONST_MAXHEALTH", {0, 1}},
	{"CONST_NULLIFIERDAMAGEAMT", {0, 1}},
	{"CONST_NULLIFIERDAMAGES", {0, 1}},
	{"CONST_PACKETREQUESTDELAY", {0, 1}},
	{"CONST_PIXELCOORDX", {0, 1}},
	{"CONST_PIXELCOORDY", {0, 1}},
	{"CONST_REQUESTACPACKETS", {0, 1}},
	{"CONST_REQUESTPACKETS", {0, 1}},
	{"CONST_SHOWAMMOACBAR", {0, 1}},
	{"CONST_SHOWAMMOBAR", {0, 1}},
	{"CONST_SHOWHEALTHBAR", {0, 1}},
	{"CONST_SNIPERTARGET", {0, 1}},
	{"CONST_SNIPERIGNORELOS", {0, 1}},
	{"CONST_SUPPORTSDIGITALIS", {0, 1}},
	{"CONST_TAKEMAPSPACE", {0, 1}},
	{"CONST_THORTARGET", {0, 1}},

	#pragma region Math Commands
	{"abs", {1, 1}},
	{"acos", {1, 1}},
	{"approximately", {2, 1}},
	{"asfloat", {1, 1}},
	{"asin", {1, 1}},
	{"asint", {1, 1}},
	{"atan", {1, 1}},
	{"atan2", {2, 1}},
	{"ceil", {1, 1}},
	{"cos", {1, 1}},
	{"E", {0, 1}},
	{"floor", {1, 1}},
	{"ln", {1, 1}},
	{"log", {2, 1}},
	{"log10", {1, 1}},
	{"max", {2, 1}},
	{"min", {2, 1}},
	{"PI", {0, 1}},
	{"pow", {2, 1}},
	{"round", {2, 1}},
	{"ShortestAngle", {2, 1}},
	{"sin", {1, 1}},
	{"sqrt", {1, 1}},
	{"tan", {1, 1}},
	#pragma endregion

	{"delay", {1, 0}},

	#pragma region Timers
	{"GetTimer0", {0, 1}},
	{"SetTimer0", {1, 0}},
	{"GetTimer1", {0, 1}},
	{"SetTimer1", {1, 0}},
	{"GetTimer2", {0, 1}},
	{"SetTimer2", {1, 0}},
	{"GetTimer3", {0, 1}},
	{"SetTimer3", {1, 0}},
	#pragma endregion

	#pragma region Input Commands
	{"GetKey", {1, 1}},
	{"GetKeyDown", {1, 1}},
	{"GetMouseButton", {1, 1}},
	{"GetMouseButtonDown", {1, 1}},
	{"GetMousePosition", {0, 2}},
	{"GetMouseScreenPosition", {0, 2}},
	{"EnableAlternateControlMode", {1, 0}},
	{"EnableNormalMouse", {1, 0}},
	{"GetKeyUp", {1, 1}},
	{"GetMouseButtonUp", {1, 1}},
	{"GetMouseCell", {0, 2}},
	{"GetMouseScreenPixelPosition", {0, 2}},
	{"EnableNormalKeyInput", {1, 0}},
	#pragma endregion

	#pragma region Script Commands
	{"AddScriptToUnit", {2, 0}},
	{"GetScriptVar", {3, 1}},
	{"SetScriptVar", {4, 0}},
	#pragma endregion

	#pragma region Unit Commands
	{"Self", {0, 1}},
	{"GetUnitAttribute", {2, 1}},
	{"SetUnitAttribute", {3, 0}},
	{"CurrentCoords", {0, 2}},
	{"CurrentX", {0, 1}},
	{"CurrentY", {0, 1}},
	#pragma endregion

	#pragma region Image Commands
	{"SetImage", {3, 0}},
	{"SetImagePosition", {5, 0}},
	{"SetImagePositionX", {3, 0}},
	{"SetImagePositionY", {3, 0}},
	{"SetImagePositionZ", {3, 0}},
	{"SetImageScale", {4, 0}},
	{"SetImageScaleX", {3, 0}},
	{"SetImageScaleY", {3, 0}},
	{"SetImageRotation", {3, 0}},
	{"SetImageColor", {6, 0}},
	{"GetImagePosition", {2, 3}},
	{"GetImagePositionX", {2, 1}},
	{"GetImagePositionY", {2, 1}},
	{"GetImagePositionZ", {2, 1}},
	{"GetImageScale", {2, 2}},
	{"GetImageScaleX", {3, 1}},
	{"GetImageScaleY", {3, 1}},
	{"GetImageRotation", {2, 1}},
	{"GetImageColor", {2, 4}},
	{"RemoveImages", {1, 0}},
	#pragma endregion

	#pragma region Creeper Commands
	{"AddCreeper", {3, 0}},
	{"DamageCreeper", {6, 0}},
	{"SetCreeperNoLower", {3, 0}},
	{"IsCreeperInRange", {7, 1}},
	{"SetCreeper", {3, 0}},
	{"GetCreeper", {2, 1}},
	#pragma endregion

	#pragma region Utility Commands
	{"GetGameTimeFrames", {0, 1}},
	#pragma endregion

	#pragma region Utility Commands
	{"SetPopupText", {1, 0}},
	{"SetPopupTextAlwaysVisible", {1, 0}},
	{"SetPopupTextX", {1, 0}},
	{"SetPopupTextY", {1, 0}},
	{"SetText", {1, 0}},
	{"SetTextAnchor", {1, 0}},
	{"SetTextColor", {4, 0}},
	{"SetTextSize", {1, 0}},
	{"SetTextX", {1, 0}},
	{"SetTextY", {1, 0}},
	#pragma endregion
};

void Converter::newLine() {
	buffer << '\n';
	for(int i = 0; i < depth; i++) {
		buffer << '\t';
	}
}

Types Converter::resolveType(const AstNode& node) {
	if(dynamic_cast<const IntLit*>(&node) || dynamic_cast<const BoolLit*>(&node)) {
		return Types::Int;
	}
	if(dynamic_cast<const FloatLit*>(&node)) {
		return Types::Float;
	}
	if(dynamic_cast<const StringLit*>(&node)) {
		return Types::String;
	}
	if(auto op = dynamic_cast<const Operation*>(&node)) {
		auto left = resolveType(*(op->Left));
		auto right = resolveType(*(op->Right));

		if(left == Types::String || right == Types::String) {
			return Types::String;
		}
		if(left == Types::Float || right == Types::Float) {
			return Types::Float;
		}
		if(left == Types::Int || right == Types::Int) {
			return Types::Int;
		}
		return Types::Unknown;
	}
	if(auto var = dynamic_cast<const Variable*>(&node)) {
		auto v = findVar(var->Name);
		if(v) {
			return resolveType(*(v->Value));
		}
	}
	return Types::Unknown;
}

const LetStatement* Converter::findVar(const std::string& name) {
	for(int i = scopes.size() - 1; i >= 0; i--) {
		auto& scope = scopes[i];
		if(scope.count(name)) {
			return scope[name];
		}
	}
	return nullptr;
}

void Converter::visit(const AstNode* node, bool discard) {
	node->visit(*this, discard);
}

void Converter::visit(const ReturnStatement& node) {
	if(node.ReturnValue) {
		visit(node.ReturnValue, false);
	}
	buffer << "return ";
	newLine();
}

void Converter::visit(const LoopStatement& node) {
	// TODO: var declaration in Init
	visit(node.Init, true);
	buffer << "while ";
	visit(node.Condition, false);
	if(stack != 1) {
		LogError(Error, node.Location, "Stack error: too many parameters for loop condition");
	}
	buffer << "repeat ";
	visit(node.Body, true);
	visit(node.Repeat, true);
	buffer << "endwhile";
	assert(stack == 0);
	newLine();
}

void Converter::visit(const DoLoopStatement& node) {
	LogError(Error, node.Location, "Not implemented");
}

void Converter::visit(const IfStatement& node) {
	visit(node.Condition, false);
	if(stack == 0) {
		LogError(Error, node.Location, "Stack error: missing parameter for if condition");
	} else if(stack != 1) {
		LogError(Error, node.Location, "Stack error: too many parameters for if condition");
	}
	buffer << "if";
	stack--;

	depth++;
	newLine();
	visit(node.If, true);
	assert(stack == 0);
	if(node.Else) {
		depth--;
		newLine();
		buffer << "else";
		depth++;
		newLine();
		visit(node.Else, true);
		assert(stack == 0);
	}
	depth--;
	newLine();
	buffer << "endif";
	newLine();
}

void Converter::visit(const Scope& node) {
	scopes.emplace_back();
	for(auto&& i : node.Body) {
		visit(i, true);
		assert(stack == 0);
	}
	auto scope = scopes.back();
	scopes.pop_back();
	for(auto&& n : scope) {
		if(n.second->Type == Tokentype::Let) {
			buffer << "--" << n.second->getName();
			newLine();
		}
	}
}

void Converter::visit(const LetStatement& node) {
	if(node.Type == Tokentype::Const) {
		LogError(Error, node.Location, "Input variable only allowed at root level");
	}
	if(node.Value) {
		visit(node.Value, false);
		assert(stack == 1);
	} else {
		buffer << "0";
	}
	auto& scope = scopes.back();
	if(scope.count(node.Name)) {
		LogError(Error, node.Location, "Variable " + node.Name + " already defined");
	}
	if(findVar(node.Name)) {
		LogError(Warning, node.Location, "Declaration of '" + node.Name + "' hides previous local declaration");
	}
	if(node.Type == Tokentype::Var) {
		scopes[0][node.Name] = &node;
	}
	scope[node.Name] = &node;
	buffer << "->" + node.getName() + "";
	stack--;
	newLine();
}

void Converter::visit(const FunctionDecl& node) {
	LogError(Error, node.Location, "Nested functions not allowed");
}

void Converter::visit(const SelectExpression& node, bool discard) {
	int startStack = stack;
	visit(node.Condition, false);
	assert(stack - startStack == 1);
	buffer << "if ";
	stack = startStack;
	visit(node.True, discard);
	int lCount = stack - startStack;
	buffer << "else ";
	stack = startStack;
	visit(node.False, discard);
	int rCount = stack - startStack;
	if(lCount != rCount) {
		LogError(Error, node.Location, "Stack error: left and right side of ternary expression don't return the same amount of arguments");
	}
	buffer << "endif ";
	if(discard) {
		assert(stack == 0);
	}
}

void Converter::visit(const PreExpression& node, bool discard) {
	visit(node.Left, discard);
	if(discard) {
		assert(stack == 0);
		return;
	}

	switch(node.Type) {
		case Tokentype::Inc:
		case Tokentype::Dec:
			LogError(Error, node.Location, "++ and -- Currently not supported");
			break;
		case Tokentype::Add: break;
		case Tokentype::Sub: buffer << "neg ";
			break;
		case Tokentype::LNot: buffer << "not ";
			break;
		case Tokentype::Not: buffer << "neg 1 sub ";
			break;
		default: throw std::runtime_error("Not reachable");
	}
}

void Converter::visit(const PostExpression& node, bool discard) {
	LogError(Error, node.Location, "++ and -- Currently not supported");
}

void Converter::visit(const Operation& node, bool discard) {
	if(node.Type == Tokentype::Assign) {
		auto var = dynamic_cast<Variable*>(node.Left);
		if(var) {
			visit(node.Right, false);
			auto v = findVar(var->Name);
			if(v) {
				if(!discard) {
					buffer << "dup ";
					stack++;
				}
				buffer << "->" << v->getName() << " ";
				stack--;
			} else {
				LogError(Error, node.Location, "Undefined variable " + var->Name);
			}
		} else {
			LogError(Error, node.Location, "Left side of an assign has to be a variable");
		}
		return;
	}

	if(discard) {
		visit(node.Left, true);
		assert(stack == 0);
		visit(node.Right, true);
		assert(stack == 0);
		return;
	}

	int stackStart = stack;
	visit(node.Left, false);
	if(stack - stackStart == 0) {
		LogError(Error, node.Left->Location, "left side of operation returns no arguments");
	} else if(stack - stackStart > 1) {
		LogError(Error, node.Left->Location, "left side of operation returns too many arguments");
	}

	stackStart = stack;
	visit(node.Right, false);
	if(stack - stackStart == 0) {
		LogError(Error, node.Left->Location, "right side of operation returns no arguments");
	} else if(stack - stackStart > 1) {
		LogError(Error, node.Left->Location, "right side of operation returns too many arguments");
	}

	switch(node.Type) {
		case Tokentype::LOr:
			buffer << "or ";
			break;
		case Tokentype::LAnd:
			buffer << "and ";
			break;
		case Tokentype::NullCoal:
			buffer << "swap dup if swap pop else pop endif ";
			break;
		case Tokentype::Equals:
			buffer << "eq ";
			break;
		case Tokentype::NEquals:
			buffer << "neq ";
			break;
		case Tokentype::Less:
			buffer << "lt ";
			break;
		case Tokentype::LessEq:
			buffer << "lte ";
			break;
		case Tokentype::Greater:
			buffer << "gt ";
			break;
		case Tokentype::GreaterEq:
			buffer << "gte ";
			break;
		case Tokentype::Or:
		case Tokentype::Xor:
		case Tokentype::And:
			throw "Not implemented";
		case Tokentype::LShift:
			buffer << "2 swap pow mul ";
		case Tokentype::RShift:
			buffer << "2 swap pow div ";
		case Tokentype::Sub:
			buffer << "sub ";
			break;
		case Tokentype::Mul:
			buffer << "mul ";
			break;
		case Tokentype::Pow:
			buffer << "pow ";
			break;
		case Tokentype::Div:
			buffer << "div ";
			break;
		case Tokentype::Mod:
			buffer << "mod ";
			break;
		case Tokentype::Add:
			if(resolveType(node) == Types::String) {
				buffer << "concat ";
			} else {
				buffer << "add ";
			}
			break;
		default:
			throw std::runtime_error("Not reachable");
	}
	stack--;
}

void Converter::visit(const FunctionCall& node, bool discard) {
	if(auto var = dynamic_cast<Variable*>(node.Left)) {
		int stackStart = stack;
		for(auto&& param : node.Params) {
			visit(param, false);
		}
		int count = stack - stackStart;

		if(functions.count(var->Name)) {
			buffer << "@" << var->Name;
			if(discard) {
				// TODO: check for return count
				// buffer << "ClearStack ";
			}
		} else if(nativeVars.count(var->Name)) {
			auto& func = nativeVars[var->Name];
			buffer << var->Name;
			if(count < func.consumes) {
				LogError(Error, var->Location, "Missing arguments for function \"" + var->Name + '"');
			} else if(count > func.consumes) {
				LogError(Error, var->Location, "Too many arguments for function \"" + var->Name + '"');
			}
			if(discard && func.returns > 0) {
				buffer << "ClearStack ";
			}
			stack = stackStart + func.returns;
		} else {
			LogError(Error, var->Location, "Unknown function \"" + var->Name + '"');
		}
		newLine();
	} else {
		LogError(Error, node.Location, "Left side of function call has to be function name");
	}
}

void Converter::visit(const IntLit& node, bool discard) {
	if(!discard) {
		buffer << node.Value << " ";
		stack++;
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
		stack++;
	}
}

void Converter::visit(const StringLit& node, bool discard) {
	if(!discard) {
		buffer << '\"' << node.Value << "\" ";
		stack++;
	}
}

void Converter::visit(const BoolLit& node, bool discard) {
	if(!discard) {
		buffer << (node.Value ? "TRUE " : "FALSE ");
		stack++;
	}
}

void Converter::visit(const Variable& node, bool discard) {
	auto v = findVar(node.Name);
	if(v) {
		buffer << "<-" << v->getName() << ' ';
		stack++;
	} else {
		if(nativeVars.count(node.Name)) {
			// TODO: if (function is const && discard) the call can be skipped
			auto var = nativeVars[node.Name];
			buffer << node.Name << " ";
			if(var.consumes != 0) {
				LogError(Error, node.Location, node.Name + " is a function and not a variable");
			}
			stack += var.returns;
			if(discard && stack != 0) {
				buffer << "ClearStack ";
			}
		} else {
			LogError(Error, node.Location, "Undefined variable " + node.Name);
		}
	}
}

std::string crpl(const std::vector<AstNode*>& ast, const std::string& fileName) {
	Converter cnvrt;
	FunctionDecl* main = nullptr;

	for(auto node : ast) {
		if(auto func = dynamic_cast<FunctionDecl*>(node)) {
			if(func->Name == "main") {
				main = func;
				continue;
			}
			if(cnvrt.functions.count(func->Name)) {
				LogError(Error, node->Location, "function \"" + func->Name + "\" exists multiple times");
			}
			cnvrt.functions[func->Name] = func;
		} else if(auto var = dynamic_cast<LetStatement*>(node)) {
			if(var->Type == Tokentype::Const) {
				cnvrt.buffer << '$' << var->Name << ':';
				cnvrt.scopes[0][var->Name] = var;

				// TODO: do better const eval
				if(dynamic_cast<IntLit*>(var->Value) || dynamic_cast<FloatLit*>(var->Value) || dynamic_cast<StringLit*>(var->Value) || dynamic_cast<BoolLit*>(var->Value)) {
					cnvrt.visit(var->Value, false);
					cnvrt.stack = 0;
					cnvrt.newLine();
					continue;
				}
				if(const auto op = dynamic_cast<PreExpression*>(var->Value)) {
					if(op->Type == Tokentype::Sub && (dynamic_cast<IntLit*>(op->Left) || dynamic_cast<FloatLit*>(op->Left))) {
						cnvrt.buffer << "-";
						cnvrt.visit(op->Left, false);
						cnvrt.stack = 0;
						cnvrt.newLine();
						continue;
					}
				}
				LogError(Error, var->Location, "Right site of input variable has to be simple constant");
			}
		}
	}

	if(!main) {
		LogError(Error, fileName, "No main function found");
	}

	cnvrt.buffer << "once";
	cnvrt.depth = 1;
	cnvrt.newLine();

	for(auto node : ast) {
		if(dynamic_cast<FunctionDecl*>(node)) {
			continue;
		}
		if(auto l = dynamic_cast<LetStatement*>(node)) {
			if(l->Type == Tokentype::Const) {
				continue;
			}
		}
		cnvrt.visit(node, true);
	}
	cnvrt.depth = 0;
	cnvrt.newLine();
	cnvrt.buffer << "endonce\n\n";

	cnvrt.visit(main->Body, true);

	for(auto node : ast) {
		auto func = dynamic_cast<FunctionDecl*>(node);
		if(!func || func->Name == "main") {
			continue;
		}
		cnvrt.depth = 0;
		cnvrt.newLine();

		cnvrt.depth = 1;
		cnvrt.buffer << ':' << func->Name;
		cnvrt.newLine();

		for(auto param : func->Params) {
			throw std::logic_error("Not implemented");
		}

		cnvrt.visit(func->Body, true);
	}

	return cnvrt.buffer.str();
}
