local test;

function set(a)	
	test = a;
end

function get(a)
	return test;
end

function Init()
	test = {9, 8, 7}
	--MessageBox(test);
	return 1;
end

function Release()
--	MessageBox("Release");
	return 1;
end

function Update(a)
	return 1;
end