def fact(num):
    factorial = 1
    for i in range(1, num + 1):
        factorial *= i
    return factorial

def permutations(digits):
    result = []

    if len(digits) == 0:
        return result
    if len(digits) == 1:
        return [digits[:]]

    for i in range(len(digits)):
        n = digits[:1]
        digits = digits[1:]
        perms = permutations(digits)

        for perm in perms:
            perm += n
        
        result += perms
        digits += n
    return result

def permListToString(digitsList):
    for i in range(len(digitsList)):
        digitsList[i] = ''.join(digitsList[i])
    return digitsList