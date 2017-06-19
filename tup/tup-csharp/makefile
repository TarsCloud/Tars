#[20120522 v2.1.0]
#1 增加tup精简版的支持
#[20120508 v2.0.2]
#1 解决float和double类型解码错误的bug；
#2 解决long类型的负数超过short和int类型大小时的bug；
#3 解决csharp显示打印嵌套List类型时死循环的问题；
#4 解决c#读取List类型错误的bug 


VERSION=2.1.0

TARGET=csharp_$(VERSION)

FILES :=`ls -I "makefile" -I "$(TARGET)_tmp_dir"`

tar: clean
	@if [ -d $(TARGET)_tmp_dir ]; then \
		echo "dir has exist:$(TARGET)_tmp_dir, abort."; \
		exit 1; \
	else \
		mkdir -p $(TARGET)_tmp_dir/$(TARGET);\
		echo $(FILES);\
		cp -rf $(FILES) $(TARGET)_tmp_dir/$(TARGET)/; \
		cd $(TARGET)_tmp_dir; tar --exclude=".svn" --exclude="_svn" -czvf $(TARGET).tgz $(TARGET)/; cd ..; \
		if [ -f "$(TARGET).tgz" ]; then \
			mv -vf $(TARGET).tgz $(TARGET).`date +%Y%m%d%H%M%S`.tgz; \
		fi; \
		mv $(TARGET)_tmp_dir/$(TARGET).tgz ./; \
		rm -rf $(TARGET)_tmp_dir; \
		echo "tar cvfz $(TARGET).tgz ..."; \
	fi
clean:
	-@rm *.tgz