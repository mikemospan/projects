import combinatorics
import ast

DIGITS = 4
TARGET = 10
operators = ['+', '-', '*', '/', '**']

# if the permutation contains stacked indicies, then ignore the calculation (too large)
def canCalculate(line):
    prev = False
    for i in range(len(line)):
        if not line[i].isnumeric() and (line[i] != '(' and line[i] != ')'):
            if line[i] == "*" and line[i+1] == "*":
                if prev == True:
                    return False
                prev = True
            elif line[i] != "*":
                prev = False
    return True

# permutates all mathematical operators including brackets
def permutateSymbols(digits):
    count1 = 0
    count2 = 0
    count3 = 0

    for perm in range(len(operators) ** (DIGITS - 1)):
        line = digits[0] + operators[count1] + digits[1] + operators[count2] + digits[2] + operators[count3] + digits[3]
        result = permutateBrackets(line)
        
        if result != "Not found":
            return result + ' = 10'

        count1 += 1
        if count1 == len(operators):
            count1 = 0
            count2 += 1
        if count2 == len(operators):
            count1 = 0
            count2 = 0
            count3 += 1
    return "Not found"

# introduces a new permutatation with two brackets i.e (a ** b) * (c - d)
def doubleBrackets(line):
    numDigits = 0
    newLine = ''
    for i in range(len(line)):
        if line[i].isnumeric():
            if numDigits % 2 == 0:
                newLine += '('
                newLine += line[i]
            else:
                newLine += line[i]
                newLine += ')'
            numDigits += 1
        else:
            newLine += line[i]
    return newLine

# permutates every combination of single opening and closing brackets
def permutateBrackets(line):
    for i in range(len(line)):
        if line[i].isnumeric():
            openBrackets = line[:i] + '(' + line[i:]
            for j in range(len(openBrackets)):
                if openBrackets[j].isnumeric() and j - i > 2:
                    closeBrackets = openBrackets[:j+1] + ')' + openBrackets[j+1:]
                    try:
                        node = ast.parse(closeBrackets, mode='eval')
                        if canCalculate(closeBrackets) and eval(compile(node, '<string>', mode='eval')) == TARGET:
                            return closeBrackets
                    except ZeroDivisionError:
                        continue

    newLine = doubleBrackets(line)
    try:
        node = ast.parse(newLine, mode='eval')
        if canCalculate(newLine) and eval(compile(node, '<string>', mode='eval')) == TARGET:
            return newLine
        else:
            return "Not found"
    except ZeroDivisionError:
        return "Not found"

# permutates all digits
def permutateDigits(digits):
    listPerms = combinatorics.permutations(list(digits))
    stringPerms = combinatorics.permListToString(listPerms)
    for perm in stringPerms:
        permutation = permutateSymbols(perm)
        if permutation != "Not found":
            return permutation
    return "Not found"

digits = input()
print(permutateDigits(digits))
