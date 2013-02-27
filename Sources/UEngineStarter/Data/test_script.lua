local test;

function set(a)	
	test = a;
	return 1;
end

function get(a)

	return test;
end

function Init()
	test[1].m1 = test[1].m1 + test[1].m2;
	return 1;
end

function Release()
--	MessageBox("Release");
	return 1;
end

function Update(a)
	return 1;
end