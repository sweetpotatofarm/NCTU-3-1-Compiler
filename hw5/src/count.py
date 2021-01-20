str1=''
s = input()
while s!='#':
    str1 = str1+s+'\n'
    s = input()

c_flag = 0
ans = 0
for i in str1:
    if(i=="\""):
        c_flag = 1-c_flag
    if(i=="," and c_flag==0):
        ans += 1

sstr = ''
for i in range (ans):
    sstr+="%s"
print("%s num is: ",ans)
print(sstr)