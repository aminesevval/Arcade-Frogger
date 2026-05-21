from pathlib import Path
text = Path('main.c').read_text(encoding='utf-8')
stack=[]
for idx,ch in enumerate(text,1):
    if ch == '{':
        stack.append((idx, '{'))
    elif ch == '}':
        if not stack:
            print('Extra closing brace at', idx)
            break
        stack.pop()
else:
    if stack:
        print('Unclosed brace count', len(stack), 'first at', stack[-1])
    else:
        print('Braces balanced')
