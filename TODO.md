
### `DONE` cpp

   调用外部cpp

   待：选项过滤

   gcc -E 会删除多余空白符，导致列号不准。

   -traditional-cpp 选项可以保留空白和注释。

### `DONE` lexer:fsync

   写单元测试，针对fsync函数进行大批量测试
   
   待：#pragma等支持

### `DONE` 完成driver，调用外部预处理器

   Mac OS X 上不能直接调用/usr/bin/cpp，总是会报错，跟 Availability.h 有关。

   应该调用 c99 -E，man c99 可以看到这是一个标准C编译器

   待：as、ld的调用

### `TODO` 移除tname，改用register_print_function


### `TODO` 完成词法分析器

   词法分析器接受的输入来自于预处理器的输出

   遵循UNIX哲学，词法分析器从标准输入(stdin)读入
   
   C预处理器格式

   # 行号 文件名 其他

   每行第一个字符总是 #。例如：

   # 3 "/usr/include/stdio.h" 

   上面的示例表示该行的下面一行是 stdio.h 的第3行

   目前有这三部分足矣，<其他>部分待查

   完成情况统计

   1. 注释：// 和 /###/，完成
   2. 关键词：C99共37个，完成
   3. 标识符：identifiers，完成
   4. 操作符和分隔符：完成
   5. 常量：
   
      5.1 字符常量
      
      5.2 整数常量：完成

      	  八进制：完成
	  十六进制：完成
	  十进制：完成

      5.3 浮点数常量

      5.4 字符串常量
   

### `TODO` 完成表达式的语法分析