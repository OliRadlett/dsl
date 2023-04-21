var x = 1
var y = 2
var a = 1

z() {
	var x = 2
	x = 3
	y = 3
}

if (x == 1) {

	z()

}

isequal(var i, var ii) {

	if (i == ii) {

		print("Equal!")

	}

}

var i = 1
var ii = 1
isequal(i, ii)


var test = [1, 2, 3, 4]
print(test)
var p = "test"
print(p)
print("TEST!")