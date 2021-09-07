~~~c
gdb -- The Gnu Debugger
gcc:
    把优化关掉 -Oo  -O[n] (n=1,2,3...)显示优化信息
    补充调试信息  -g
    加警告 -wall

gdb命令
   l [文件名：][行号 | 函数] 查看文件  -- list
   run / r -- 运行程序
   break/b 行号/函数名 -- 遇到声明跳过
   info break (i b) --看断点信息
   print/p &arr(想看的信息) -- 看监视
   display arr (自动显示表达内容)  / undisplay 编号 (关闭)
   next/n -- 单步调试F10
   step/s -- 单步调试F11
   continue/c 运行到下一个断点
   delete 断点编号 (删除某个断点)
   disable 断点编号(断点失效)
   enable 断点编号(断点生效)
   ignore 1 4 (断点1忽略4次)
看内存：
    x/3xw arr
    x/12xb arr
查看调用堆栈
    backtrack/bt
        
1.core --黑匣子
  出现错误时野指针和栈溢出引起的时候
  1.ulimit -c unlimited   -- 解除core filesize的限制
    cat /proc/sys/kernel/core-pattern  -- 看位置
  2.重新使用-g选项编译
    	$ gcc error1.c -o error1 -g
  3.执行生成core文件
        ./error
   4.$ gdb error core
        bt 看调用堆栈信息
        
设置命令行错误
        set args 后加想看的信息
        show args
        
2.栈溢出错误
~~~

