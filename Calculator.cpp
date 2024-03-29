#include <iostream>
#include <stack>
#include <string>
#include <algorithm> 
#include <queue>

// Checks for Operators
bool isOperator(char op) 
{
    return op == '+' || op == '-' || op == '*' || op == '/';
}

// Operation Available
void operationStackFunc(std::stack<int>& numberStack, char operation) 
{

    int A{ numberStack.top() };
    numberStack.pop();

    int B{ numberStack.top() };
    numberStack.pop();

    switch (operation) 
    {
    case '+':
        numberStack.push(A + B);
        break;
    case '-':
        numberStack.push(A - B);
        break;
    case '*':
        numberStack.push(A * B);
        break;
    case '/':
        numberStack.push(A / B);
        break;
    default:
        std::cout << "????\n";
    }

}

void operationQueueFunc(std::queue<int>& numberQueue, char operation) 
{
    int A{ numberQueue.front() };
    numberQueue.pop();

    int B{ numberQueue.front() };
    numberQueue.pop();

    switch (operation) {
    case '+':
        numberQueue.push(A + B);
        break;
    case '-':
        numberQueue.push(A - B);
        break;
    default:
        std::cout << "???\n";
    }
}

std::string evaluateExpression(const std::string& expression) 
{

    // Iniatiliazing Stacks
    std::stack<char>operationStack;
    std::stack<int>numberStack;

    for (const char& character : expression)
    {
        // Parenthesis are Opossite because the string is reversed

       // Skip if it's a space
       if (character == ' ') { continue; }

       // Mark as inside a parenthesis
       if (character == ')') 
       {
           operationStack.push(character);
           continue;
       }

       // Calculate the prior Operation
       else if (character == '(') 
       {

           // Go through all the string between the last ( and this )
           while (operationStack.top() != ')' && !operationStack.empty()) 
           {

               // Calculation here
               operationStackFunc(numberStack, operationStack.top());
               operationStack.pop();

           }

           // Make sure that we delete '(' from the stack
           operationStack.pop();

       }

       // Operators
       else if (isOperator(character)) 
       {
           operationStack.push(character);
       }

       // Numbers
       else 
       {
           // Conversion from char to int (ASCII table)
           numberStack.push(character - 48);
       }
    }


    std::cout << "-----------------------------------\n";

    // Initializing Queues
    std::queue<char>operationQueue;
    std::queue<int>numberQueue;

    // Looping through the string, calculate everything WITHOUT any order
    while(!operationStack.empty()) 
    {
        // If the operator has priority --> * or /
        if (operationStack.top() == '*' || operationStack.top() == '/') 
        {
            // Calculate the number and let him in the stack
            operationStackFunc(numberStack, operationStack.top());

            // Pop the operation --> the * or /
            operationStack.pop();
        }
        // If the operator doesn't have priority --> + or -
        else 
        {
            // Number goes in a queue
            numberQueue.push(numberStack.top());
            numberStack.pop();

            // Operator goes in another queue
            operationQueue.push(operationStack.top());
            operationStack.pop();
        }
    }

    // Final number in the stack that must be added to the Queue
    numberQueue.push(numberStack.top());
    numberStack.pop();
    
    // Loop through the final Queue
    while (!operationQueue.empty())
    {
        // Calculate operations using + and -
        operationQueueFunc(numberQueue, operationQueue.front());
        operationQueue.pop();
    }

    // Return the last number in the queue --> which is the final result
    return std::to_string(numberQueue.front());
}

// Function to reverse the string (made in a function because it looks cool)
void reverse(std::string& expression) 
{
    std::reverse(expression.begin(), expression.end());
}

int main() {
    std::cout << "Enter an expression to calculate: ";
    std::string expression;
    std::getline(std::cin, expression);

    // Reverse the string --> work with std::stack
    reverse(expression);

    // Make all the Calculations + output
    std::string result{ evaluateExpression(expression) };
    std::cout << "\nFinal result is " << result << '\n';

    return 0;
}
