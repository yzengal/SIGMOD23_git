from random import randint
from random import shuffle

s = "01"

def genOneSignature(L):
	m = len(s)
	ids = [randint(0,m-1) for i in xrange(L)]
	ret = "".join(map(lambda id:s[id], ids))
	return ret
	
def genMultipleSignature(filename, n, L):
	st = set()
	with open(filename, "w") as fout:
		fout.write("%d %d\n" % (n, L))
		while n>0:
			s = genOneSignature(L)
			if s not in st:
				n -= 1
				st.add(s)
				fout.write("%s\n" % (s))
	
if __name__ == "__main__":
	filename = "./dataset_synthetic"
	n = 10**6
	L = 48
	genMultipleSignature(filename, n, L)
	