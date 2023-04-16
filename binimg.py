import png
import sys
import getopt

def main(args):
    width = 128
    infile = ""
    outfile = ""

    opts, args = getopt.getopt(args, "i:w:o:")
    for opt, arg in opts:
        if opt == '-i':
            infile = arg
        elif opt == '-o':
            outfile = arg
        elif opt == '-w':
            width = int(arg)
        else:
            print("ERROR: wrong argument format")
            return
    
    print(width)
    print(infile)
    print(outfile)

    with open(infile, mode="rb") as f:
        bin_data = f.read()

    rows = []

    while len(bin_data) > 0:
        rows = rows + [ bin_data[0:width-1] ]
        bin_data = bin_data[width:]

    png.from_array(rows, 'L').save(outfile)

if __name__ == "__main__":
   main(sys.argv[1:])