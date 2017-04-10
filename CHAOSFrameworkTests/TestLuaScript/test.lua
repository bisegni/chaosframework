function testString(str_to_test)
	if(string.match(str_to_test, "test_string")) then
		print(" String"..str_to_test.." has been matched");
	else
		print(" String"..str_to_test.." didn't match");
	end
end