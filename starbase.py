
import sys,os,re, subprocess, tempfile

from threading import Thread

"""
        Add colnum
        Add insert header before, after other header value
        Add add/del header
        Add add/del row
        Add add/del col

        Add row formatting
"""


try:
    import starbase_data
    Starbase_readdata = starbase_data.readdata

except ImportError:

    def Starbase_readdata(fp, type, strip) :
        if ( strip == True ) :
	    return [[typ(val.strip()) for (typ, val) in zip(type, row.rstrip("\n").split("\t"))] for row in fp.readlines()]
	else :
	    return [[typ(val)         for (typ, val) in zip(type, row.rstrip("\n").split("\t"))] for row in fp.readlines()]
        
class Starbase(object):
    """
        # Starbase Data Tables in python
        # 
        # Read a table from disk:
        #
        >>> tab = Starbase("input.tab")

        #
        # A table may be read from a command pipeline by placing "|" as the 
        # first character in the filename:
        #
        >>> tab = Starbase('| row < input.tab "X > 3"')

        #
        # A table may be read from an open file descriptor:
        #
        >>> fp = open("input.tab")
        >>> tab = Starbase(fp)

        #
        # The data is stored as a list of lists.  It can be accessed directly
        # from the Starbase object.  Row and column indexing is zero based as
        # in python.  Columns may be indexed by integer or column name string:
        #
        >>> tab[0][0]                   # get value at 0, 0
        '1'

        #
        # The values are stored as strings by default.  An optional keyword
        # patameter "types" may be used to add data types to columns.  This 
        # makes using tables in expressions less painful.
        #
        tab = Starbase("| jottable 10 x y z", types = { "x" : float, "y" : int })

        x = tab[0].x + tab[0].y
        print x
        2

        >>> tab[2][1] = 5               # set value at 0, 0

        >>> tab[1]["Y"]                 # get value at row 1 col "Y"
        '2'

        >>> tab[4]["Y"] = 9             # set value at row 5 col "Y"

        #
        # Rows can be dotted too.
        #
        >>> tab[4].Y
        '9'

        >>> tab[4].Y = 8

        #
        # Table header values may be accessed by using python "dot" notation or
        # indexing the Starbase object with a string. Note that header values are
        # arrays and thus need to be indexed just like rows:
        #
        >>> tab.label = "label string"  # set header value

        >>> tab.label[0]                # or
        'label string'

        >>> tab["label"][0]
        'label string'

        #
        #
        # Iterating over the table returns each row of the table in turn:
        #
        >>> for row in tab :
        ...     print row
        ['1', '1', '1']
        ['2', '2', '2']
        ['3', '5', '3']
        ['4', '4', '4']
        ['5', '8', '5']

        #
        # Rows can be sliced and iterated over:
        #
        >>> for row in tab[0:2] :
        ...     print row
        ['1', '1', '1']
        ['2', '2', '2']

        #
        # The row itself can be iterated of:
        #
        >>> for val in tab[2] :
        ...     print val
        3
        5
        3

        # 
        # Slice the rows of a data column.  This works as an a notation to 
        # select a column vector for input to numpy array().
        #
        >>> tab[:].X
        ['1', '2', '3', '4', '5']

        # 
        # Or slice a few rows of the column:
        #
        >>> tab[0:2].X
        ['1', '2']

        #
        # Alternative "arrays" constructor can be used to create
        # a starbase object from a bunch of python list or numpy
        # array data.
        #
        >>> print Starbase.arrays("XXX", [1, 2, 3], "Y", [3, 4, 5])     # doctest: +NORMALIZE_WHITESPACE
        XXX     Y
        ---     -
        1       3
        2       4
        3       5

        # 
        # Keyword arguments are also supported, but the order of 
        # columns in the starbase table is determined by the 
        # python hash not the order passed to the constructor.
        #
        >>> print Starbase.arrays(X=[1, 2, 3], Y=[3, 4, 5])             # doctest: +NORMALIZE_WHITESPACE
        Y       X
        -       -
        3       1
        4       2
        5       3

        # 
        # The Starbase table may be printed directly.  This can safely be used
        # for "small" tables (less than several megabytes).  For truely huge
        # tables, the ">" operator will iterativly print the table to a file
        # descriptor and may be faster.
        #
        >>> print tab                   # print table                   # doctest: +NORMALIZE_WHITESPACE
        label   label string
        X       Y       Z
        -       -       -
        1       1       1
        2       2       2
        3       5       3
        4       4       4
        5       8       5

        >>> fp = open('/tmp/output', 'w')
        >>> print >> fp, tab            # print to open file fp
        
        #
        >>> tab > sys.stdout            # write table to sys.stdout     # doctest: +NORMALIZE_WHITESPACE
        label   label string
        X       Y       Z
        -       -       -
        1       1       1
        2       2       2
        3       5       3
        4       4       4
        5       8       5

        >>> tab > "output.tab"          # write table to file named "output.tab"

        #
        # If the output file name passed to ">" begins with "|" the table will be
        # filtered through a shell pipeline:
        #
        >>> t = (Starbase("| jottable 10 x") > "| row 'x <= 3'")
        >>> t > sys.stdout
        x
        -
        1
        2
        3

        #
        # Rows and header values have an independent existence and can be
        # selected and assigned:
        #
        >>> row = tab[4]

        >>> row[2] = 4                  # set column 3 of row 6 to 4

        >>> tab[0] = tab[3]             # copy row 3 to row 0

        # Union of two tables
        #
        >>> print (t + t)
        x
        -
        1
        2
        3
        1
        2
        3

        # Difference of two tables
        #
        >>> print (tab > "| sorttable -u X") % "X=x" - t        # doctest: +NORMALIZE_WHITESPACE
        label   label string
        x
        -
        4
        5
        """


    class StarbaseHdr(object):
        """ Enables off by 1 access to the header value list reference. 
        """

        def __init__(self, data) :
            self.data = data

        def __getitem__(self, indx) :
            if ( type(indx) == slice ) :
                return self.data[1:][indx]

            return self.data[indx+1]
            
        def __setitem__(self, indx, value) :
            self.data[indx+1] = str(value)

        def __str__(self) :
            return "\t".join(self.data[1:])


    # StarbaseRow is a friend of Starbase and accesses its __Private members.
    #
    class StarbaseRow(object):
        """ Enables column lookup by string value.  Holds the reference to a row
          and the column dictionary.
        """

        def __init__(self, tabl, indx) :
            self.__tabl = tabl
            self.__indx = indx

            self.__initialized = 1

        def __str__(self) :
            if ( type(self.__indx) == list or type(self.__indx) == tuple ) :
		reply = ""
		for i in self.__indx:
		    reply = reply + "\t".join((str(item) for item in self.__tabl._Starbase__data[i])) + "\n"

		return reply

            return "\t".join((str(item) for item in self.__tabl._Starbase__data[self.__indx]))

        def __getitem__(self, indx) :
            if ( type(indx) == list or type(indx) == tuple ) :
		xidx = []
		for i in indx :
		    if ( type(i) == str ) :
			i = self.__tabl._Starbase__indx[i]

		    xidx.append(i)

	        indx = xidx

            if ( type(self.__indx) == slice ) :
		if ( type(indx) == list or type(indx) == tuple ) :
                    return [[row[i] for i in indx] for row in self.__tabl._Starbase__data.__getitem__(self.__indx)]

                return [row[indx] for row in self.__tabl._Starbase__data.__getitem__(self.__indx)]

            if ( type(self.__indx) == list or type(self.__indx) == tuple ) :
		if ( type(indx) == list or type(indx) == tuple ) :
		    reply = []
		    for row in self.__indx:
			reply.append([self.__tabl._Starbase__data[row][i] for i in indx])

		    return reply

                return [row[indx] for row in self.__tabl._Starbase__data.__getitem__(self.__indx)]


	    if ( type(indx) == list or type(indx) == tuple ) :
		return [self.__tabl._Starbase__data[self.__indx][i] for i in indx]

            return self.__tabl._Starbase__data[self.__indx][indx]
            
        def __setitem__(self, indx, value) :
            if ( type(indx) == str ) :
                indx = self.__tabl._Starbase__indx[indx]

            self.__tabl._Starbase__data[self.__indx][indx] = self.__tabl._Starbase__type[indx](value)

        def __getattr__(self, indx) :
            return self.__getitem__(indx)

        def __iter__(self) :
            return self.__tabl._Starbase__data[self.__indx].__iter__()

        def __setattr__(self, indx, value) :
            if ( not self.__dict__.has_key("_StarbaseRow__initialized") \
              or self.__dict__.has_key(indx) ) :
                self.__dict__[indx] = value
                return

            self.__setitem__(indx, value)

	def __length_hint__(self) :
	    return 0

	def where(self, expr):
            if ( type(self.__indx) == list ) :
		i = 0
		indx = []
		for i in self.__indx :
		    row = self.__tabl._Starbase__data.__getitem__(i)

		    if eval(expr, {} , dict(zip(self.__tabl._Starbase__cols, row))):
			indx.append(i)

		    i = i + 1

		if ( len(indx) == 1 ) :
		    indx = indx[0]

		return Starbase.StarbaseRow(self.__tabl, indx)

            if ( type(self.__indx) == slice ) :
		i = 0
		indx = []
		for row in self.__tabl._Starbase__data.__getitem__(self.__indx) :
		    if eval(expr, {} , dict(zip(self.__tabl._Starbase__cols, row))):
			indx.append(i)

		    i = i + 1

		if ( len(indx) == 1 ) :
		    indx = indx[0]

		return Starbase.StarbaseRow(self.__tabl, indx)

    class StarbasePipeWriter(Thread) :

        def __init__(self, table, write) :
            Thread.__init__(self)
            self.table = table
            self.write = write

        def run(self) :
            self.table > self.write
            self.write.close()


    def __init__(self, fp=None, types={}, dtype=str, strip=False) :
        if ( fp == None ) :
            return

        if ( type(fp) == str ) :
            fp = open(fp, "rU") if ( fp[0:1] != "|" ) else os.popen(fp[1:], "rU")

        self.__head = {}
        self.__line = []
        self.__type = []
        self.__cols = []

        self.__headline = fp.readline().rstrip().split("\t")
        self.__dashline = fp.readline().rstrip().split("\t")

        dashes = len([s for s in self.__dashline if re.match('-+' , s.strip())])

        # Read lines until the dashline is found
        #
        while ( not dashes                                                                      \
                 or dashes != len([s for s in self.__headline if re.match('\w+', s.strip())]) ) :
            if ( re.match('\w+', self.__headline[0].strip()) ) :
                self.__head[self.__headline[0].strip()] = len(self.__line)

            self.__line.append(self.__headline)

            self.__headline = self.__dashline
            self.__dashline = fp.readline().rstrip().split("\t")

	    dashes = 0
	    for s in self.__dashline:
		if re.match('^-+$' , s.strip()):
		    dashes += 1


        i = 0

        self.__indx = {}
        for col in self.__headline :
            col = col.strip()

            self.__indx[col] = i
            self.__type.append(types[col] if ( types.has_key(col) ) else dtype)
	    self.__cols.append(col)
            i += 1

        # Read the data in, converting to types
        #
        self.__data = Starbase_readdata(fp, self.__type, strip)

        self.__initialized = 1


 
    @classmethod
    def arrays(self, *args, **kwargs) :
        self = Starbase()
        args = list(args)

        self.__head = {}
        self.__line = []
        self.__type = []
        self.__headline = []
        self.__dashline = []

        for col in kwargs :
            args.append(col)
            args.append(kwargs[col])

        i = 0
        vals = []
        self.__indx = {}
        while i < len(args) :
            col = args[i]
            val = args[i+1]

            self.__indx[col] = i
            self.__headline.append(col)
            self.__dashline.append("-" * len(col))

            vals.append(val)

            i += 2

        arry = [val for val in vals]
        self.__data = zip(*arry)

        self.__initialized = 1
        return self

    def __str__(self) :
        # Cast the table as a string.
        #
        return ( "\n".join(["\t".join(row) for row in self.__line]) + "\n" if self.__line else "" ) \
               + "\t".join(self.__headline) + "\n"                                              \
               + "\t".join(self.__dashline) + "\n"                                              \
               + "\n".join(("\t".join((str(item) for item in row)) for row in self.__data))

    def __iter__(self) :
        return self.__data.__iter__()

    def __getitem__(self, indx) :
        if ( type(indx) == str ) :
            return Starbase.StarbaseHdr(self.__line[self.__head[indx]])

        return Starbase.StarbaseRow(self, indx)

    def __setitem__(self, indx, value) :
        if ( type(indx) == str ) :
            if ( not self.__head.has_key(indx) ) :
                self.__head[indx] = len(self.__line)
                self.__line.append([indx])
    
            if ( type(value) == list ) :
                self.__line[self.__head[indx]][1:] = [str(v) for v in value]
            else :
                self.__line[self.__head[indx]][1:] = [str(value)]

            return

        if ( value.__class__.__name__ == 'StarbaseRow' ) :
            value = value._StarbaseRow__tabl.__data[value._StarbaseRow__indx]

        if ( type(value) != list ) :
            raise TypeError("Starbase set row expected list")

        if ( len(self.__headline) != len(value) ) :
            raise TypeError("Starbase set row expected list of " + str(len(self.__headline)) )
                
        self.__data[indx] = [typ(val) for (typ, val) in zip(self.__type, value)]

    def __getattr__(self, indx) :
        return Starbase.StarbaseHdr(self.__line[self.__head[indx]])

    def __setattr__(self, indx, value) :
        if ( not self.__dict__.has_key("_Starbase__initialized")        \
          or self.__dict__.has_key(indx) ) :
            self.__dict__[indx] = value
            return

        self.__setitem__(indx, value)

    def __binop(self, other, command) : 
        fd, file1 = tempfile.mkstemp()
        os.close(fd)
        fd, file2 = tempfile.mkstemp()
        os.close(fd)

        self  > file1
        other > file2

        return Starbase(command + " " + file1 + " " + file2)

    def __add__(self, other) :
        return self.__binop(other, "| uniontable")

    def __or__(self, other) :
        return self.__binop(other, "| uniontable")

    def __sub__(self, other) :
        return self.__binop(other, "| diffrtable")

    def __and__(self, other) :
        return self.__binop(other, "| intertable")

    def __mod__(self, columns) :
        if ( type(columns) == list or type(columns) == tuple ) :
            columns = " ".join(columns)
        
        return (self > ("| column " + columns))

    def __invert__(self) :
        return self > "| transposetable"

    def __floordiv__(self, columns) :
        if ( type(columns) == list or type(columns) == tuple ) :
            columns = " ".join(columns)

        return self > ("| sorttable -u " + columns)

    def __gt__(self, file) :
        if ( type(file) == str ) :
            if ( file[0:1] == "|" ) :
                if ( file[0:1] == "|" ) :
                    p = subprocess.Popen(file[1:], shell=True, bufsize=1        \
                            , stdin=subprocess.PIPE                             \
                            , stdout=subprocess.PIPE                    \
                            , stderr=subprocess.STDOUT, close_fds=True)

                    writer = Starbase.StarbasePipeWriter(self, p.stdin)

                #if ( file[0:2] == ".:" ) :
                #    writer = Starbase.StarbaseSokWrite(self, file)

                writer.start()

                reply = Starbase(p.stdout)

                writer.join()

                return reply

            file = open(file, "w")

        for line in self.__line :
            print >> file, "\t".join(line)

        print >> file, "\t".join(self.__headline)
        print >> file, "\t".join(self.__dashline)
        for row in self.__data :
            print >> file, "\t".join((str(item) for item in row))

        return None

    def __rshift__(self, file) :
        file = open(file, "a")

        self > file


if __name__ == '__main__':
    # jottable 5 X Y Z > input.tab
    #
    import doctest
    doctest.testmod()
