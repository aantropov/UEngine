local test;

function set(a)
	test = a;
end

function get(a)
	return test;
end

function Init()
	test = {four = 4, five = 5, ['six'] = 6}
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