from random import randint
import os

class constForGen(object):
	nQ = 200
	kList = [5, 10, 20, 50, 100]
	rList = [0.02, 0.04, 0.08, 0.16, 0.32, 0.64]
	
	
class CFG(constForGen):
	pass
	

def genStringParameters(dirname, n, L):
	if not os.path.exists(dirname):
		os.mkdir(dirname)	
	# gen query object
	filname = os.path.join(dirname, "q.txt")
	with open(filname, "w") as fout:
		fout.write("%d\n" % (CFG.nQ))
		qList = [randint(0,n-1) for i in xrange(CFG.nQ)]
		fout.write("%s\n" % (" ".join(map(str, qList))))
	# gen radius list	
	filname = os.path.join(dirname, "r.txt")
	with open(filname, "w") as fout:
		fout.write("%d\n" % (len(CFG.rList)))
		radList = [CFG.rList[i]*L for i in xrange(len(CFG.rList))]
		fout.write("%s\n" % (" ".join(map(str, radList))))
	# gen k-integer list	
	filname = os.path.join(dirname, "k.txt")
	with open(filname, "w") as fout:
		fout.write("%d\n" % (len(CFG.kList)))
		fout.write("%s\n" % (" ".join(map(str, CFG.kList))))	
		
def genColorParameters(dirname, n, dmax):
	if not os.path.exists(dirname):
		os.mkdir(dirname)	
	# gen query object
	filname = os.path.join(dirname, "q.txt")
	with open(filname, "w") as fout:
		fout.write("%d\n" % (CFG.nQ))
		qList = [randint(0,n-1) for i in xrange(CFG.nQ)]
		fout.write("%s\n" % (" ".join(map(str, qList))))
	# gen radius list	
	filname = os.path.join(dirname, "r.txt")
	with open(filname, "w") as fout:
		fout.write("%d\n" % (len(CFG.rList)))
		radList = [CFG.rList[i]*dmax for i in xrange(len(CFG.rList))]
		fout.write("%s\n" % (" ".join(map(str, radList))))
	# gen k-integer list	
	filname = os.path.join(dirname, "k.txt")
	with open(filname, "w") as fout:
		fout.write("%d\n" % (len(CFG.kList)))
		fout.write("%s\n" % (" ".join(map(str, CFG.kList))))	
		
	
def exp0(dirname):
	genStringParameters(os.path.join(dirname, "dutch"), 229328, 40)
	genStringParameters(os.path.join(dirname, "synthetic"), 10**6, 50)
	genStringParameters(os.path.join(dirname, "english"), 466550, 45)
	genColorParameters(os.path.join(dirname, "color"), 112682, 610)
	
if __name__ == "__main__":
	dirname = "./dataset/"
	exp0(dirname)
	