import cpp

class SymbolConstructor extends Constructor {
  SymbolConstructor() {
    this.getDeclaringType().getName() = "symbol" and
    this.getNumberOfParameters() = 1 and
    this.getParameter(0).getType().toString() = "const char *"
  }
}

from FunctionCall call, Function callerFunc, SymbolConstructor ctor, string literalArg, string file, string callerClass
where
    call.getTarget() = ctor and
    exists(StringLiteral lit | call.getArgument(0) = lit and lit.getValue() = literalArg) and
    call.getFile().getRelativePath() = file and
    call.getEnclosingFunction() = callerFunc and
    not (callerFunc.getName() = "install_tactics" and call.getFile().getBaseName() = "install_tactic.cpp") and
    if exists(callerFunc.getDeclaringType()) then callerClass = callerFunc.getDeclaringType().getName() else callerClass = "<global>"
select call, callerFunc, ctor, literalArg, file, callerClass
