def printVector(arr):
 
    if (len(arr) != 1):
        for i in range(len(arr)):
            print(arr[i], end = " ")
        print()
        
def findWays(arr, i, n):
    if (n == 0):
        printVector(arr)
    for j in range(i, n + 1):
        if (j > 3):
            continue
        arr.append(j)
        findWays(arr, j, n - j)
        del arr[-1]

if __name__ == '__main__':
    n = 10
    arr = []
    findWays(arr, 1, n)