#!/usr/bin/env python
# Echo client program
import socket
import time

class madc_control:
    def __init__(self, host='mvme_slow', port=12345):
        self.comm = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.comm.connect((host, port))
        self.sequence = 0
        self.num_boards()

    def send(self, command, params=""):
        out_str = '%d %s %s ' % (self.sequence, command, params)
        self.sequence += 1
        bytes = self.comm.send(out_str)
        if (bytes != len(out_str)):
            print 'Send: only sent %d bytes of: %s' % (bytes, out_str)
            return
        in_str = self.comm.recv(1024)
        temp = in_str.split()
        in_seq = int(temp[0])
        in_cmd = temp[1]
        ret = temp[2:]
        if in_seq != self.sequence - 1:
            print 'Send: Sequence error, sequence=%d, in_seq=%d' % (self.sequence, in_seq)
            return
        if (ret[0] == 'SEQUENCE'):
            print 'Send: Sequence error on receiving side sequence=%d, in_seq=%d' % (self.sequence, in_seq)
            return
        if (ret[0] == 'INVALID'):
            print 'Send: Not a valid command on receiving side sequence=%d, in_seq=%d' % (self.sequence, in_seq)
            return
        if (ret[0] == 'RUNNING'):
            print 'Send: Acquisition running and receiver cannot run command sequence=%d, in_seq=%d' % (self.sequence, in_seq)
            return
        if in_cmd != command:
            print 'Send: Command error, command=%d, in_cmd=%d' % (command, in_cmd)
            return
        if (ret[0] != 'OK'):
            print 'Send: Error at receiving side = %s' % in_str
        return ret[1:]

    def begin(self):
        ret = self.send('begin')
        self.start = int(ret[0]) + 0.000001*int(ret[1])
        print "DAQ started at UTC=%f" % self.start
        
    def end(self):
        ret = self.send('end')
        self.stop = int(ret[0]) + 0.000001*int(ret[1])
        self.total_bytes = int(ret[2])
        print "DAQ ended at UTC=%f with %d bytes" % (self.stop, self.total_bytes)
        print "This is %f secs for %f kB/s" % (self.stop - self.start,
                                               self.total_bytes/1024/(self.stop - self.start))
        
    def close(self):
        ret = self.send('close')

    def set_trig_thresh(self, thresh):
        if (thresh > 956): thresh = 956
        if (thresh < 0): thresh = 0
        ret = self.send('trig_thresh', params="%d" % thresh)

    def num_boards(self):
        ret = self.send('num_boards')
        self.boards = int(ret[0])

    def firmware(self, board):
        if board < 0: board = 0
        if board >= self.boards: board = self.boards-1
        ret = self.send('firmware', params="%d" % board)
        return int(ret[0])

    def set_resolution(self, board, resolution):
        if board < 0: board = 0
        if board >= self.boards: board = self.boards-1
        if resolution < 0: resolution = 0
        if resolution > 4: resolution = 4
        ret = self.send('resolution', params="%d %d" % (board, resolution))

    def set_threshold(self, board, channel, threshold):
        if board < 0: board = 0
        if board >= self.boards: board = self.boards-1
        if channel < 0: channel = 0
        if channel >= 32: channel = 31
        ret = self.send('threshold', params="%d %d %d" % (board, channel, threshold))

#changed adc 5 channel 1 threshold per E. Wulf 5/29/09

config = [[4,
           [60, 60, 60, 60, 60, 60, 60, 60, \
            60, 60, 60, 60, 60, 60, 60, 60, \
            60, 60, 60, 60, 60, 60, 60, 60, \
            60, 60, 60, 60, 60, 60, 60, 60]],
          [0,
           [20, 20, 20, 20, 20, 20, 20, 20, \
            20, 20, 20, 20, 20, 20, 20, 20, \
            20, 20, 20, 20, 20, 20, 20, 20, \
            20, 20, 20, 20, 20, 20, 20, 20]],
          [0,
           [20, 20, 20, 20, 20, 20, 20, 20, \
            20, 20, 20, 20, 20, 20, 20, 20, \
            20, 20, 20, 20, 20, 20, 20, 20, \
            20, 20, 20, 20, 20, 20, 20, 20]],
          [0,
           [20, 20, 20, 20, 20, 20, 20, 20, \
            20, 20, 20, 20, 20, 20, 20, 20, \
            20, 20, 20, 20, 20, 20, 20, 20, \
            20, 20, 20, 20, 20, 20, 20, 20]],
          [0,
           [20, 20, 20, 20, 20, 20, 20, 20, \
            20, 20, 20, 20, 20, 20, 20, 20, \
            20, 20, 20, 20, 20, 20, 20, 20, \
            20, 20, 20, 20, 20, 20, 20, 20]]]
          

if __name__ == '__main__':
    m = madc_control()
    for board, values in enumerate(config):
        print "Board=%d has firmware version=%s" % (board, hex(m.firmware(board)))
        m.set_resolution(board, values[0])
        for channel, thresh in enumerate(values[1]):
            m.set_threshold(board, channel, thresh)
    
    m.set_trig_thresh(200)
    m.begin()
    time.sleep(10)
    m.end()
    m.close()
