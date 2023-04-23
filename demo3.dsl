var listone = [1, 2, 3, 4]
var listtwo = [1, 1, 1, 2, 3, 4]
var listthree = [1, 1, 1, 1]
var listfour = [2, 3, 4, 5]

print("countif() x = 1")
print(listone)
print(countif(listone, x -> x == 1))
print(listtwo)
print(countif(listtwo, x -> x == 1))

print("all() x = 1")
print(listthree)
print(all(listthree, x -> x == 1))
print("all() x != 1")
print(listthree)
print(all(listthree, x -> x != 1))
print("all() x = 1")
print(listtwo)
print(all(listtwo, x -> x == 1))
print("all() x != 1")
print(listfour)
print(all(listfour, x -> x != 1))

print("exists() x = 1")
print(listone)
print(exists(listone, x -> x == 1))
print("exists() x = 1")
print(listthree)
print(exists(listthree, x -> x == 1))
print("exists() x = 2")
print(listthree)
print(exists(listthree, x -> x == 2))