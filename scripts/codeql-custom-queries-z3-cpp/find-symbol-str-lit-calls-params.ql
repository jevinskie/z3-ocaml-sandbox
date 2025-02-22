import cpp

// void params::set_sym(symbol const & k, symbol const & v);
// void params::set_sym(char const * k, symbol const & v);
// void params_ref::set_sym(symbol const & k, symbol const & v);
// void params_ref::set_sym(char const * k, symbol const & v);
// solver_p.set_sym("gc", symbol("dyn_psm"));

class SymbolCstrConstructor extends Constructor {
  SymbolCstrConstructor() {
    this.getDeclaringType().getName() = "symbol" and
    this.getNumberOfParameters() = 1 and
    this.getParameter(0).getType().toString() = "const char *"
  }
}

class SymbolCstrLitCall extends ConstructorCall {
  SymbolCstrLitCall() {
    exists(SymbolCstrConstructor tgt | this.getTarget() = tgt) and
    exists(StringLiteral lit | this.getArgument(0) = lit)
  }
  string getStrLit() {
    result = this.getArgument(0).getValue()
  }
}

class SymbolEmptyConstructor extends Constructor {
  SymbolEmptyConstructor() {
    this.getDeclaringType().getName() = "symbol" and
    this.getNumberOfParameters() = 0
  }
}

class SymbolEmptyCall extends ConstructorCall {
  SymbolEmptyCall() {
    exists(SymbolEmptyConstructor tgt | this.getTarget() = tgt)
  }
}

class SymbolNullAccess extends VariableAccess {
  SymbolNullAccess() {
    this.getNameQualifier().getQualifyingElement().getName() = "symbol" and
    this.getNameQualifier().getQualifiedElement().toString() = "null"
  }
}

class SetSymFunc extends MemberFunction {
  SetSymFunc() {
    this.getName() = "set_sym" and
    this.getDeclaringType().getName() in ["params", "params_ref"] and
    this.getNumberOfParameters() = 2
  }
}

class SetSymFuncCallBase extends FunctionCall {
  SetSymFuncCallBase() {
    exists(SetSymFunc tgt | this.getTarget() = tgt)
  }
}

class SetSymFuncCallComptimeKeyBase extends SetSymFuncCallBase {
  SetSymFuncCallComptimeKeyBase() {
    (exists(StringLiteral lit | this.getArgument(0) = lit) or exists(SymbolCstrLitCall ktgt | this.getArgument(0) = ktgt))
  }
  string getKeySym() {
    if exists(StringLiteral lit | this.getArgument(0) = lit) then result = this.getArgument(0).getValue() else result = this.getArgument(0).getValue()
  }
}

class SetSymFuncCallComptimeValBase extends SetSymFuncCallBase {
  SetSymFuncCallComptimeValBase() {
    exists(SymbolCstrLitCall vcall | this.getArgument(1) = vcall)
  }
  string getValSym() {
    exists(SymbolCstrLitCall vcall | this.getArgument(1) = vcall) and
    result = this.getArgument(1).getValue()
  }
}

class SetSymFuncCallComptimeKeyComptimeVal extends SetSymFuncCallComptimeKeyBase, SetSymFuncCallComptimeValBase {
  SetSymFuncCallComptimeKeyComptimeVal() {
    (exists(StringLiteral lit | this.getArgument(0) = lit) or exists(SymbolCstrLitCall ktgt | this.getArgument(0) = ktgt)) and
    exists(SymbolCstrLitCall vcall | this.getArgument(1) = vcall)
  }
}

class SetSymFuncCallComptimeKey extends SetSymFuncCallBase {
  SetSymFuncCallComptimeKey() {
    (exists(StringLiteral lit | this.getArgument(0) = lit) or exists(SymbolCstrLitCall ktgt | this.getArgument(0) = ktgt)) and
    not exists(SymbolCstrLitCall vcall | this.getArgument(1) = vcall)
  }
  string getKeySym() {
    if exists(StringLiteral lit | this.getArgument(0) = lit) then result = this.getArgument(0).getValue() else result = this.getArgument(0).getValue()
  }
}

from SymbolCstrLitCall call, Function callerFunc, SymbolCstrConstructor ctor, string literalArg, string file, string callerClass, string expr
where
    call.getTarget() = ctor and
    exists(StringLiteral lit | call.getArgument(0) = lit and lit.getValue() = literalArg) and
    call.getFile().getRelativePath() = file and
    call.getEnclosingFunction() = callerFunc and
    not (callerFunc.getName() = "install_tactics" and call.getFile().getBaseName() = "install_tactic.cpp") and
    if exists(callerFunc.getDeclaringType()) then callerClass = callerFunc.getDeclaringType().getName() else callerClass = "<global>" and
    exists(call.getEnclosingStmt()) and
    expr = call.getEnclosingStmt().toString()
select expr, call, callerFunc, ctor, literalArg, file, callerClass
