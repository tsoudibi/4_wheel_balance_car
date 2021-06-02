import matplotlib.pyplot as pyplot

read_continue = True
i = 0

class ReadLine:

    def __init__(self, s):
        self.buf = bytearray()
        self.s = s

    print('hello')

    def readline(self):
        i = self.buf.find(b"\n")
        if i >= 0:
            r = self.buf[:i + 1]
            self.buf = self.buf[i + 1:]
            return r
        while True:
            i = max(1, min(2048, self.s.inWaiting()))
            data = self.s.read(i)
            i = data.find(b"\n")
            if i >= 0:
                r = self.buf + data[:i + 1]
                self.buf[0:] = data[i + 1:]
                return r
            else:
                self.buf.extend(data)

    def getData(self, ser):

        x = []
        y = []

        # fig = pyplot.figure(figsize=(5, 3))
        fig = pyplot.figure()
        ax = fig.add_subplot(111)
        line1, = ax.plot(x, y, 'o-', lw=10, markersize=20)

        pyplot.xlabel('X')
        pyplot.ylabel('Y')
        pyplot.title('Center of mass')
        pyplot.axis([-2000, 2000, -2000, 2000])
        pyplot.grid(axis='y')
        pyplot.ion()
        pyplot.show()
        pyplot.tight_layout()

        j = 0

        try:
            while read_continue:
                while ser.inWaiting():
                    j += 1
                    test = ReadLine(ser)
                    data = test.readline()
                    data = data.decode()
                    data = data.split(',')
                    x.append(int(data[0]))
                    y.append(int(data[1]))
                    if j > 3:
                        x.pop(0)
                        y.pop(0)
                    line1.set_data(x, y)
                    ax.autoscale_view(True, True, True)
                    fig.canvas.draw()
                    pyplot.pause(0.00001)

            # ser.close()  # close serial
            pyplot.close(fig)  # close figure
            print('goodbye')

        except KeyboardInterrupt:
            pyplot.close(fig)  # close figure
            print('goodbye')
