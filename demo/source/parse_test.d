module parse_test;

import dparse.ast;
import std.stdio, std.range;
import forge.fmt;

class TestVisitor : ASTVisitor
{
    alias visit = ASTVisitor.visit;
    int indentLevel;

    void writeIndent()
    {
         write(' '.repeat(indentLevel * 4));
    }

    override void visit(const FunctionDeclaration decl)
    {
        writeIndent();
        println("{}", decl.name.text);
        indentLevel++;
        scope (exit) indentLevel--;
        decl.accept(this);
    }
    override void visit(const BaseClass bases)
    {
        writeIndent();
        println("{}", bases.type2.typeIdentifierPart.identifierOrTemplateInstance.identifier.text);
    }
    override void visit(const ClassDeclaration decl)
    {
        println("{}", decl.name.text);
        if (decl.baseClassList !is null)
        {
            indentLevel++;
            scope (exit) indentLevel--;
            decl.baseClassList.items[0].accept(this);
        }
        decl.accept(this);
    }
}

import dparse.lexer;
import dparse.parser : parseModule;
import dparse.rollback_allocator : RollbackAllocator;

void driveParseTest()
{
    auto sourceCode = q{
        class MyClass : Base
        {

        }
        void foo() @safe {
            void bar();
        }
    };

    LexerConfig config;
    auto cache = StringCache(StringCache.defaultBucketCount);
    auto tokens = getTokensForParser(sourceCode, config, &cache);

    RollbackAllocator rba;
    auto m = parseModule(tokens, "test.d", &rba);
    scope auto visitor = new TestVisitor();
    visitor.visit(m);
}
