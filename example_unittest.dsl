// Example unit test written in my DSL
// Define a list with 3 1's and then
// use the countif function to count
// the number of 1's in the list.
// Compare the result of countif to
// the expected result of 3. 
//=====================================

var list = [1, 1, 1, 2, 3, 4]
var expected = 3
var actual = countif(list, x -> x == 1)

if (actual == expected) {

	print("Test passed")
	
}

if (actual != expected) {
	
	print("Test failed")

}