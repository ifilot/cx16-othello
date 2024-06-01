# parse .hlp file into binary file that can be parsed by the Othello game

def main():
    pages = []
    with open('help.hlp') as f:
        lines = f.readlines()
        
        for line in lines:
            if line.startswith('#'):
                continue

            if line.startswith('|PAGE'):
                parsing = True
                curpage = bytearray()
                continue
            
            if line.startswith('|ENDPAGE|'):
                parsing = False
                curpage.extend([0x20] * (20 * 12 - len(curpage)))
                pages.append(curpage)
                continue
            
            if parsing:
                line = line.strip()
                line = line.replace('_', ' ')
                if len(line) > 20:
                    curpage += line[0:20].encode('ascii')
                else:
                    curpage += line.encode('ascii') + bytearray([0x20] * (20 - len(line)))

    with open('HELP.DAT', 'wb') as f:
        for page in pages:
            f.write(page)
    
if __name__ == '__main__':
    main()