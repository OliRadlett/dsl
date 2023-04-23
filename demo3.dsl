var listone = [1, 2, 3, 4]
var listtwo = [1, 1, 1, 2, 3, 4]
var listthree = [1, 1, 1, 1]
var listfour = [2, 3, 4, 5]

print("countif() x = 1")
print(listone)
print(countif(listone, x -> x == 1))
print(listtwo)
print(countif(listtwo, x -> x == 1))

print("all()")
all(listthree, x -> x == 1)
all(listthree, x -> x != 1)
all(listtwo, x -> x == 1)
all(listfour, x -> x != 1)

print("exists()")
exists(listone, x -> x == 1)
exists(listthree, x -> x == 1)
exists(listthree, x -> x == 2)

print(print("test"))