var list = [1, 1, 1, 2, 3, 4]
var expected = 3
var actual = countif(list, x -> x == 1)

if (actual == expected) {

	print("Test passed")
	
}

if (actual != expected) {
	
	print("Test failed")

}