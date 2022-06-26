if __name__ == '__main__':
    with open('test/testfiles/spam_client.txt', 'r') as r:
        with open('test/testfiles/spam_client_expected.txt', 'w+') as w:
            for num, line in enumerate(r):
                if num < 64: continue
                if line.startswith('MSG:'):
                    line = line.split(':', 2)
                    w.write(f'{line[1]}: {line[2]}')
                elif line.startswith('LIST:'):
                    line = line.split(':', 1)
                    w.write(f'(current chatters: '+ line[1].strip("\n") + ')\n')
                elif line.startswith('ENTER:'):
                    line = line.split(':', 1)
                    w.write(f'(' + line[1].strip("\n") + ' has entered the chat)\n')
                elif line.startswith('LEAVE:'):
                    line = line.split(':', 1)
                    w.write('(' + line[1].strip("\n") + ' has left the chat)\n')
