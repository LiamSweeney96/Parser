#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.c"
#include "lexer.h"
#include "parser.h"

int closeParentheses = 0, openParentheses = 0;
// you can declare prototypes of parser functions below
ParserInfo multiIdent(ParserInfo pi);
void error(char* s, ParserInfo pi);
ParserInfo classDeclar();
ParserInfo memberDeclar();
ParserInfo classVarDeclar();
ParserInfo type();
ParserInfo subroutineDeclar();
ParserInfo paramList();
ParserInfo subroutineBody();
ParserInfo statement();
ParserInfo varDeclarStatement();
ParserInfo letStatement();
ParserInfo ifStatement();
ParserInfo whileStatement();
ParserInfo doStatement();
ParserInfo subroutineCall();
ParserInfo expressionList();
ParserInfo returnStatemnt();
ParserInfo expression();
ParserInfo relationalExpression();
ParserInfo ArithmeticExpression();
ParserInfo term();
ParserInfo factor();
ParserInfo operand();
int openParenCheck(ParserInfo pi);
int closeParenCheck(ParserInfo pi);
int openBraceCheck(ParserInfo pi);
int closeBraceCheck(ParserInfo pi);
int closeBracketCheck(ParserInfo pi);
int identifierCheck(ParserInfo pi);
int semiColonCheck(ParserInfo pi);
int commaCheck(ParserInfo pi);
int endOfFileCheck(ParserInfo pi);
int equalCheck(ParserInfo pi);
int isKeyword(ParserInfo pi);
ParserInfo ifElseLoop(ParserInfo pi, ParserInfo piTwo);

int openParenCheck(ParserInfo pi)
{
    // Check for open parenthesis after if
    if (strcmp(pi.tk.lx, "(") == 0)
    {
        openParentheses++;;
        return 0;
    }
    else
    {
        return 1;
    }
}
int closeParenCheck(ParserInfo pi)
{
    if (strcmp(pi.tk.lx, ")") == 0)
    {
        closeParentheses++;
        return 0;
    }
    else 
    {
        return 1;
    }
}
int openBraceCheck(ParserInfo pi)
{
    if (strcmp(pi.tk.lx, "{") == 0)
    {
        return 0;
    }
    return 1;
}
int closeBracketCheck(ParserInfo pi)
{
    if (strcmp(pi.tk.lx, "]") == 0)
    {
        return 0;
    }
    return 1;
}
int identifierCheck(ParserInfo pi)
{
    // Ensure identifier is first token
    if (pi.tk.tp == ID)
    {
        return 0;
    }
    return 1;
}
int semiColonCheck(ParserInfo pi)
{
    if (strcmp(pi.tk.lx, ";") == 0)
    {
        return 0;
    }
    return 1;
}
int endOfFileCheck(ParserInfo pi)
{
    if (pi.tk.tp == EOFile)
    {
        return 0;
    }
    return 1;
}
int commaCheck(ParserInfo pi)
{
    // If comma doesn't follow identifier, return error
    if (strcmp(pi.tk.lx, ",") == 0)
    {
        return 0;
    }
    return 1;
}
int equalCheck(ParserInfo pi)
{
    // If comma doesn't follow identifier, return error
    if (strcmp(pi.tk.lx, "=") == 0)
    {
        return 0;
    }
    return 1;
}
int isKeyword(ParserInfo pi)
{
    if (strcmp(pi.tk.lx, "var") == 0 || strcmp(pi.tk.lx, "return") == 0 || strcmp(pi.tk.lx, "if") == 0 || 
    strcmp(pi.tk.lx, "let") == 0 || strcmp(pi.tk.lx, "do") == 0 || strcmp(pi.tk.lx, "while") == 0)
    {
        return 0;
    }
    return 1;
}
ParserInfo ifElseLoop(ParserInfo pi, ParserInfo piTwo)
{
    while (strcmp(pi.tk.lx, "}") != 0)
    {
        pi = statement();
        if (pi.er == semicolonExpected)
        {
            if (strcmp(pi.tk.lx, ";") == 0)
            {
                strcpy(pi.tk.lx, ")");
                break;
            }
        }
        piTwo.tk = PeekNextToken();
        if (strcmp(piTwo.tk.lx, "}") == 0)
        {
            if (isKeyword(pi) == 1)
            {
                piTwo.tk = PeekNextToken();
                if (isKeyword(piTwo) == 1)
                {
                    pi.tk = GetNextToken();
                }
            }
            break;
        }
    }
    return pi;
}
ParserInfo multiIdent(ParserInfo pi)
{
    // Loop until parameter list end
    while (strcmp(pi.tk.lx, ";") != 0)
    {
        pi.tk = GetNextToken();  

        if (identifierCheck(pi) == 0)
        {
            pi.tk = GetNextToken();
        }
        if (strcmp(pi.tk.lx, ")") == 0)
        {
            closeParentheses++;
        }
        if (strcmp(pi.tk.lx, ";") == 0)
        {
            break;
        }
        if (commaCheck(pi) == 1)
        {
            break;
        }
    }
    return pi;
}

ParserInfo classDeclar()
{
    // Get first token
    ParserInfo pi, piTwo;
    pi.er = none;
    pi.tk = GetNextToken();

    if (pi.tk.tp == ERR)
    {
        pi.er = lexerErr;
        return pi;
    }

    // Check first token is "class"
    if (strcmp(pi.tk.lx, "class") == 0)
    {
        ;
    }
    else
    {
        pi.er = classExpected;
        return pi;
    }

    // Check second token is the class identifier
    pi.tk = GetNextToken();
    if (identifierCheck(pi) == 1)
    {
        pi.er = idExpected;
        return pi;
    }

    // Check for open brace after class declaration
    pi.tk = GetNextToken();
    if (openBraceCheck(pi) == 1)
    {
        pi.er = openBraceExpected;
        return pi;
    }

    while (pi.tk.tp != EOFile)
    {
        pi = memberDeclar();
        piTwo.tk = PeekNextToken();
        if (pi.er != none)
        {
            return pi;
        }
    }

    if (pi.er == none && (openParentheses - closeParentheses != 0))
    {
        pi.er = syntaxError;
    }
    pi.er = none;
    return pi;
}
ParserInfo memberDeclar()
{
    ParserInfo pi, piTwo;
    pi.er = none;
    
    pi.tk = PeekNextToken(); 
    piTwo.tk = PeekNextToken();

    // Check if next token is subroutine
    if (strcmp(pi.tk.lx, "constructor") == 0 || strcmp(pi.tk.lx, "function") == 0 || strcmp(pi.tk.lx, "method") == 0)
    {
        pi = subroutineDeclar();
    }
    // Check if next token is class variable
    else if (strcmp(pi.tk.lx, "static") == 0 || strcmp(pi.tk.lx, "field") == 0)
    {
        pi = classVarDeclar();
    }
    else if (strcmp(pi.tk.lx, "}") == 0)
    {
        piTwo.tk = PeekNextToken();
        if (strcmp(piTwo.tk.lx, "}") == 0)
        {
            pi.tk = GetNextToken();
            pi.tk = PeekNextToken();
            // printf("\n\nLex 1: %s | Line: %i\n\n", piTwo.tk.lx, piTwo.tk.ln);
            if (pi.tk.tp == EOFile)
            {
                pi.tk = GetNextToken();
                return pi;
            }
        }
    }
    // If token is none of the above, return error
    else
    {
        pi.er = memberDeclarErr;
        return pi;
    }
    return pi;
}
ParserInfo classVarDeclar()
{
    ParserInfo pi;
    pi.er = none;
    pi.tk = GetNextToken(); // Get static or field

    // Determine the token type
    pi = type();
    if (pi.er != none)
    {
        return pi;
    }
    pi.tk = GetNextToken(); // Get identifier

    if (identifierCheck(pi) == 1)
    {
        pi.er = idExpected;
        return pi;
    }
    pi.tk = PeekNextToken(); // Check for multiple ID or semi colon

    if (strcmp(pi.tk.lx, ";") == 0)
    {
        pi.tk = GetNextToken();
        return pi;
    }
    // Check for multiple declarations of type
    if (strcmp(pi.tk.lx, ",") == 0)
    {
        pi.tk = GetNextToken();
        return pi = multiIdent(pi);
    }
    pi.er = syntaxError;
    return pi;
}
ParserInfo type()
{
    ParserInfo pi;
    pi.er = none;
    pi.tk = GetNextToken();
    // Check the type of the token is valid
    if (strcmp(pi.tk.lx, "char") == 0 || strcmp(pi.tk.lx, "boolean") == 0 || strcmp(pi.tk.lx, "int") == 0 || pi.tk.tp == ID)
    {
        ;
    }
    else
    {
        pi.er = illegalType;
    }
    return pi;
}
ParserInfo subroutineDeclar()
{
    ParserInfo pi;
    pi.er = none;
    pi.tk = GetNextToken();  // Get constructor or function
    pi.tk = PeekNextToken(); 

    // Determine the token type
    if (strcmp(pi.tk.lx, "void") == 0)
    {
        pi.tk = GetNextToken();
    }
    else
    {
        pi = type();
        if (pi.er != none)
        {
            return pi;
        }
    }
    pi.tk = GetNextToken(); // Get ID

    if (identifierCheck(pi) == 1)
    {
        pi.er = idExpected;
        return pi;
    }
    pi.tk = GetNextToken();

    if (openParenCheck(pi) == 1)
    {
        pi.er = openParenExpected;
        return pi;
    }
     
    pi = paramList(); // Get parameters, return closing parenthesis
    
    if (pi.er != none)
    {
        return pi;
    }
    if (closeParenCheck(pi) == 1)
    {
        pi.er = closeParenExpected;
        return pi;
    }

    pi = subroutineBody();
    return pi;
}
ParserInfo paramList()
{
    ParserInfo pi, piTwo;
    pi.er = none;
    piTwo.tk = PeekNextToken();
    
    if (strcmp(piTwo.tk.lx, "{") == 0)
    {   
        pi.tk = GetNextToken();
        pi.er = closeParenExpected;
        return pi;
    }
    // Check for no parameters
    if (strcmp(piTwo.tk.lx, ")") == 0)
    {   
        pi.tk = GetNextToken();
        return pi;
    }
    pi = type(); // Get type
    
    if (pi.er != none)
    {
        return pi;
    }
    pi.tk = GetNextToken(); // Get ID after type

    if (identifierCheck(pi) == 1)
    {
        pi.er = idExpected;
        return pi;
    }
    piTwo.tk = PeekNextToken();

    // Check for parameter list end
    if (strcmp(piTwo.tk.lx, ")") == 0)
    {   
        pi.tk = GetNextToken();
        return pi;
    }
    // Check for multiple parameters
    if (commaCheck(piTwo) == 0)
    {
        pi.tk = GetNextToken();
        // Loop until parameter list end
        while (strcmp(pi.tk.lx, ")") != 0)
        {
            pi = type();
            if (pi.er != none)
            {
                return pi;
            }
            pi.tk = GetNextToken();
            if (identifierCheck(pi) == 1)
            {
                pi.er = idExpected;
                return pi;
            }
            pi.tk = GetNextToken();

            // Break loop if close parenthesis found
            if (strcmp(pi.tk.lx, ")") == 0)
            {
                break;
            }
            commaCheck(pi);
        }
    }
    return pi;
}
ParserInfo subroutineBody()
{
    ParserInfo pi, piTwo;
    pi.er = none;
    pi.tk = GetNextToken();

    if (openBraceCheck(pi) == 1)
    {
        pi.er = openBraceExpected;
        return pi;
    }
    piTwo.tk = PeekNextToken();

    // Loop through statements in subroutine
    if (strcmp(piTwo.tk.lx, "}") == 0)
    {
        pi.tk = GetNextToken();
    }
    else
    {
        while (strcmp(pi.tk.lx, "}") != 0)
        {
            piTwo.tk = PeekNextToken();

            if (strcmp(piTwo.tk.lx, "}") == 0)
            {
                pi.tk = GetNextToken();
                piTwo.tk = PeekNextToken();
                if (strcmp(piTwo.tk.lx, "}") == 0)
                {
                    pi.tk = GetNextToken();
                    piTwo.tk = PeekNextToken();
                    if (piTwo.tk.tp == EOFile)
                    {
                        pi.tk = GetNextToken();
                        pi.er = none;
                        return pi;
                    }
                }
            }
            pi = statement();
            if (pi.er != none)
            {
                return pi;
            }
            if (pi.er != none)
            {
                return pi;
            }
            if (isKeyword(pi) == 1)
            {
                pi.tk = PeekNextToken();
            }
            piTwo.tk = PeekNextToken();

            if (strcmp(pi.tk.lx, "}") == 0 && strcmp(piTwo.tk.lx, "}") == 0)
            {
                pi.tk = GetNextToken();
                piTwo.tk = PeekNextToken();
                if (isKeyword(piTwo) == 0)
                {
                    pi = statement();
                    if (pi.er != none)
                    {
                        return pi;
                    }
                    continue;
                }
                break;
            }
        }
    }
    return pi;
}
ParserInfo statement()
{
    ParserInfo pi;
    pi.tk = PeekNextToken();

    // Check which type of statement is declared
    if (strcmp(pi.tk.lx, "var") == 0)
    {
        pi = varDeclarStatement();
    }
    else if (strcmp(pi.tk.lx, "let") == 0)
    {
        pi = letStatement();
    }
    else if (strcmp(pi.tk.lx, "if") == 0)
    {
        pi = ifStatement();
    }
    else if (strcmp(pi.tk.lx, "while") == 0)
    {
        pi = whileStatement();
    }
    else if (strcmp(pi.tk.lx, "do") == 0)
    {
        pi = doStatement();
    }
    else if (strcmp(pi.tk.lx, "return") == 0)
    {
        pi = returnStatemnt();
    }
    else
    {
        pi.er = syntaxError;
    }
    return pi;
}

ParserInfo varDeclarStatement()
{
    ParserInfo pi;
    pi.er = none;
    pi.tk = GetNextToken();

    pi = type();
    if (pi.er != none)
    {
        return pi;
    }
    pi.tk = GetNextToken();

    if (identifierCheck(pi) == 1)
    {
        pi.er = idExpected;
        return pi;
    }
    pi.tk = GetNextToken();

    // Check for end of declaration
    if (strcmp(pi.tk.lx, ";") == 0)
    {
        return pi;
    }
    if (commaCheck(pi) == 0)
    {
        pi = multiIdent(pi);
    }
    return pi;
}
ParserInfo letStatement()
{
    ParserInfo pi;
    pi.er = none;
    pi.tk = GetNextToken(); // Get let
    pi.tk = GetNextToken(); // Get ID
    
    if (identifierCheck(pi) == 1)
    {
        pi.er = idExpected;
        return pi;
    }
    pi.tk = GetNextToken();

    // Check for array declaration
    if (strcmp(pi.tk.lx, "[") == 0)
    {
        pi = expression();
        if (pi.er != none)
        {
            return pi;
        }
        pi.tk = GetNextToken();
        
        if (closeBracketCheck(pi) == 1)
        {
            pi.er = closeBracketExpected;
            return pi;
        }
        pi.tk = GetNextToken();
    }
    if (equalCheck(pi) == 1)
    {
        pi.er = equalExpected;
        return pi;
    }
    pi = expression(); 

    if (pi.er != none)
    {
        return pi;
    }
    pi.tk = GetNextToken();

    if (strcmp(pi.tk.lx, ")") == 0)
    {
        closeParentheses++;
        pi.tk = GetNextToken(); 
        if (openParentheses - closeParentheses < 0)
        {
            pi.er = semicolonExpected;
            return pi;
        }
    }
    if (semiColonCheck(pi) == 1)
    {
        pi.er = semicolonExpected;
    }
    return pi;
}
ParserInfo ifStatement()
{
    ParserInfo pi, piTwo;
    pi.er = none;
    pi.tk = GetNextToken();
    pi.tk = GetNextToken();

    if (openParenCheck(pi) == 1)
    {
        pi.er = openParenExpected;
        return pi;
    }
    pi = expression();
    if (pi.er != none)
    {
        return pi;
    }
    pi.tk = GetNextToken();
    if (closeParenCheck(pi) == 1)
    {
        pi.er = closeParenExpected;
        return pi;
    }
    pi.tk = GetNextToken();
    if (openBraceCheck(pi) == 1)
    {
        pi.er = openBraceExpected;
        return pi;
    }
    piTwo.tk = PeekNextToken();
    // Check for open parenthesis after if
    if (strcmp(piTwo.tk.lx, "}") == 0)
    {
        ;
    }
    else
    {
        pi = ifElseLoop(pi, piTwo);
        if (pi.er = none)
        {
            return pi;
        }
    }
    pi.tk = PeekNextToken(); // Peek to check if next token is else
    if (strcmp(pi.tk.lx, "else") == 0)
    {
        pi.tk = GetNextToken(); // Get the else token
        pi.tk = GetNextToken();
        openBraceCheck(pi);
        pi = ifElseLoop(pi, piTwo);
    }
    return pi;
}
ParserInfo whileStatement()
{
    ParserInfo pi, piTwo;
    pi.er = none;
    pi.tk = GetNextToken(); // Get while
    pi.tk = GetNextToken(); // Get next
    if (openParenCheck(pi) == 1)
    {
        pi.er = openParenExpected;
        return pi;
    }
    pi = expression();
    if (pi.er != none)
    {
        return pi;
    }
    pi.tk = GetNextToken();
    if (closeParenCheck(pi) == 1)
    {
        pi.er = closeParenExpected;
        return pi;
    }
    pi.tk = GetNextToken();
    if (openBraceCheck(pi) == 1)
    {
        pi.er = openBraceExpected;
        return pi;
    }
    pi.tk = PeekNextToken();
    if (strcmp(pi.tk.lx, "}") == 0)
    {
        ;
    }
    else
    {
        // Loop until end of while loop
        while (strcmp(pi.tk.lx, "}") != 0)
        {
            pi = statement();
            if (pi.er != none)
            {
                return pi;
            }
            if (strcmp(pi.tk.lx, ";") == 0)
            {
                piTwo.tk = PeekNextToken();
                if (strcmp(piTwo.tk.lx, "}") == 0)
                {
                    pi.tk = GetNextToken();
                    break;
                }
            }
            if (strcmp(piTwo.tk.lx, ";") == 0)
            {
                pi.tk = GetNextToken();
            }
        }
    }
    return pi;
}
ParserInfo doStatement()
{
    ParserInfo pi, piTwo;
    pi.er = none;
    pi.tk = GetNextToken();
    pi = subroutineCall();
    if (pi.er != none)
    {
        return pi;
    }
    pi.tk = GetNextToken();
    if (semiColonCheck(pi) == 1)
    {
        pi.er = semicolonExpected;
    }
    return pi;
}
ParserInfo subroutineCall()
{
    ParserInfo pi;
    pi.er = none;
    pi.tk = GetNextToken();
    if (identifierCheck(pi) == 1)
    {
        pi.er = idExpected;
        return pi;
    }
    pi.tk = GetNextToken();
    // Check for object method calls
    if (strcmp(pi.tk.lx, ".") == 0)
    {
        pi.tk = GetNextToken();
        if (identifierCheck(pi) == 1)
        {
            pi.er = idExpected;
            return pi;
        }
        pi.tk = GetNextToken();
    }
    if (openParenCheck(pi) == 1)
    {
        pi.er = openParenExpected;
        return pi;
    }
    pi = expressionList();
    if (pi.er != none)
    {
        return pi;
    }
    pi.tk = GetNextToken();
    if (closeParenCheck(pi) == 1)
    {
        pi.er = closeParenExpected;
        return pi;
    }
    return pi;
}
// Needs to return closing bracket
ParserInfo expressionList()
{
    ParserInfo pi, piTwo;
    pi.er = none;
    piTwo.tk = PeekNextToken();

    // Check for empty string
    if (strcmp(piTwo.tk.lx, ")") == 0)
    {
        return pi;
    }
    pi = expression();
    if (pi.er != none)
    {
        return pi;
    }
    piTwo.tk = PeekNextToken();
    // Check for multiple expressions
    if (strcmp(piTwo.tk.lx, ",") == 0)
    {
        piTwo.tk = PeekNextToken();
        pi.tk = GetNextToken();
        // Loop until end of expression list
        while (strcmp(pi.tk.lx, ",") == 0)
        {
            pi = expression();
            if (pi.er != none)
            {
                return pi;
            }
            piTwo.tk = PeekNextToken();
            if (strcmp(piTwo.tk.lx, ")") == 0)
            {
                break;
            }
            pi.tk = GetNextToken();
        }
    }
    return pi;
}
ParserInfo returnStatemnt()
{
    ParserInfo pi, piTwo;
    pi.er = none;
    pi.tk = GetNextToken();  // Get return
    piTwo.tk = PeekNextToken(); // Peek next

    if (strcmp(piTwo.tk.lx, ";") == 0)
    {
        pi.tk = GetNextToken();
        return pi;
    }
    pi = expression();
    if (pi.er != none)
    {
        return pi;
    }
    if (strcmp(pi.tk.lx, "}") == 0)
    {
        pi.er = semicolonExpected;
        return pi;
    }
    pi.tk = GetNextToken();
    if (semiColonCheck(pi) == 1)
    {
        pi.er = semicolonExpected;
    }
    return pi;
}
ParserInfo expression()
{
    ParserInfo pi, piTwo;
    pi.er = none;
    pi = relationalExpression();
    if (pi.er != none)
    {
        return pi;
    }
    piTwo.tk = PeekNextToken();
    // Check to see if multiple relational expressions
    if (strcmp(piTwo.tk.lx, "&") == 0 || strcmp(piTwo.tk.lx, "|") == 0)
    {
        pi.tk = PeekNextToken();
        // Loop until end of relational expression list
        while (strcmp(pi.tk.lx, "&") == 0 || strcmp(pi.tk.lx, "|") == 0)
        {
            pi.tk = GetNextToken();
            pi = relationalExpression();
            if (pi.er != none)
            {
                return pi;
            }
        }
    }
    return pi;
}
ParserInfo relationalExpression()
{
    ParserInfo pi, piTwo;
    pi.er = none;
    pi = ArithmeticExpression(); 
    if (pi.er != none)
    {
        return pi;
    }
    piTwo.tk = PeekNextToken();
    if (strcmp(piTwo.tk.lx, "=") == 0 || strcmp(piTwo.tk.lx, ">") == 0 || strcmp(piTwo.tk.lx, "<") == 0)
    {
        pi.tk = PeekNextToken();
        while (strcmp(pi.tk.lx, "=") == 0 || strcmp(pi.tk.lx, ">") == 0 || strcmp(pi.tk.lx, "<") == 0)
        {
            pi.tk = GetNextToken();
            pi = ArithmeticExpression();
            if (pi.er != none)
            {
                return pi;
            }
        }
    }
    return pi;
}
ParserInfo ArithmeticExpression()
{
    ParserInfo pi, piTwo;
    pi.er = none;
    pi = term(); 
    if (pi.er != none)
    {
        return pi;
    }
    piTwo.tk = PeekNextToken();
    if (strcmp(piTwo.tk.lx, "+") == 0 || strcmp(piTwo.tk.lx, "-") == 0)
    {
        pi.tk = PeekNextToken();
        while (strcmp(pi.tk.lx, "+") == 0 || strcmp(pi.tk.lx, "-") == 0)
        {
            pi.tk = GetNextToken();
            pi = term();
            if (pi.er != none)
            {
                return pi;
            }
        }
    } 
    return pi;
}
ParserInfo term()
{
    ParserInfo pi, piTwo;
    pi.er = none;
    pi = factor();
    if (pi.er != none)
    {
        return pi;
    }
    piTwo.tk = PeekNextToken();
    if (strcmp(piTwo.tk.lx, "*") == 0 || strcmp(piTwo.tk.lx, "/") == 0)
    {
        pi.tk = PeekNextToken();
        while (strcmp(pi.tk.lx, "*") == 0 || strcmp(pi.tk.lx, "/") == 0)
        {
            pi.tk = GetNextToken();
            pi = factor();
            if (pi.er != none)
            {
                return pi;
            }
        }
    }
    return pi;
}
ParserInfo factor()
{
    ParserInfo pi;
    pi.er = none;
    pi.tk = PeekNextToken();
    if (strcmp(pi.tk.lx, "-") == 0 || strcmp(pi.tk.lx, "~") == 0)
    {
        pi.tk = GetNextToken();
    }
    pi = operand();
    return pi;
}
ParserInfo operand()
{
    ParserInfo pi, piTwo;
    pi.er = none;
    pi.tk = GetNextToken();
    if (strcmp(pi.tk.lx, ";") == 0)
    {
        pi.er = syntaxError;
        return pi;
    }
    if (pi.tk.tp == ID)
    {
        pi.tk = PeekNextToken();
        if (pi.tk.tp == ID)
        {
            pi.er = closeParenExpected;
            return pi;
        }
        if (strcmp(pi.tk.lx, ".") == 0)
        {
            pi.tk = GetNextToken();         // Get dot
            pi.tk = GetNextToken();         // Get ID
            if (identifierCheck(pi) == 1)   // Check ID
            {
                pi.er = idExpected;
                return pi;
            }
            pi.tk = GetNextToken();         // Get next, either "[", "(", or ";"
            if (strcmp(pi.tk.lx, "(") == 0)
            {
                openParentheses++;
            }
        }
        if (strcmp(pi.tk.lx, "[") == 0)
        {
            pi.tk = GetNextToken(); // Get open bracket
            pi = expression();      // Run expression
            if (strcmp(pi.tk.lx , ";") == 0)
            {
                pi.er = closeBracketExpected;
                return pi;
            }
            pi.tk = GetNextToken(); // Get close bracket
            if (closeBracketCheck(pi) == 1)
            {
                pi.er = closeBracketExpected;
                return pi;
            }  
        }
        if (strcmp(pi.tk.lx, "(") == 0)
        {
            pi = expressionList();  // Run expression list
            pi.tk = GetNextToken(); // Get close parenthesis
            if (closeParenCheck(pi) == 1)
            {
                pi.er = closeParenExpected;
                return pi;
            }
        }
    }
    else if (strcmp(pi.tk.lx, "(") == 0)
    {
        openParentheses++;
        pi = expression();      // Run expression
        pi.tk = GetNextToken(); // Get close parenthesis
        if (closeParenCheck(pi) == 1)
        {
            pi.er = closeParenExpected;
            return pi;
        }
    }
    else if (strcmp(pi.tk.lx, "this") == 0 || strcmp(pi.tk.lx, "true") == 0 || 
            strcmp(pi.tk.lx, "false") == 0 || strcmp(pi.tk.lx, "null") == 0 || 
            pi.tk.tp, STRING || pi.tk.tp == ID || pi.tk.tp == INT)
    {
        ;
    }
    else
    {
        pi.er = syntaxError;
    }
    return pi;
}
int InitParser (char* file_name)
{

	return 1;
}
ParserInfo Parse ()
{
	ParserInfo pi;

	// implement the function
    openParentheses = 0;
    closeParentheses = 0;
	pi = classDeclar();

	// pi.er = none;
	return pi;
}
int StopParser ()
{
	return 1;
}
#ifndef TEST_PARSER
int main ()
{

	return 1;
}
#endif