function compare_ece564_result(Round,error_tolerance)

g_result_fileID = fopen(sprintf('./input_and_output_%d/g_result.dat',Round),'r');
g_result=[];
golden_g_result=[];
while ischar(fgets(g_result_fileID,1))
    line1=fgetl(g_result_fileID);
    if find(line1=='@')
        g_result=[g_result; line1(find(line1==' ')+1:end)];
    end
end
fclose(g_result_fileID);

golden_g_result_fileID = fopen(sprintf('./input_and_output_%d/x16_g_result_ece564.dat',Round),'r');
while ischar(fgets(golden_g_result_fileID,1))
    line2=fgetl(golden_g_result_fileID);
    %if find(line2=='@')
        golden_g_result=[golden_g_result; line2(find(line2=='	')+1:end)];
    %end
end
fclose(golden_g_result_fileID);



subtraction_array=[];
for i =1:255
    subtraction_array=[subtraction_array abs((double(typecast(uint16(bin2dec(golden_g_result(i,:))),'int16'))-double(typecast(uint16(bin2dec(g_result(i,:))),'int16')))/2^15)];
end

[Val Rank] = sort(subtraction_array,'descend')
Max_subtraction = sprintf('The max subtraction is %d in place of %d', max(subtraction_array), find(subtraction_array==max(subtraction_array)))
Min_subtraction = sprintf('The min subtraction is %d in place of %d', min(subtraction_array), find(subtraction_array==min(subtraction_array)))

correct_num=length(find(subtraction_array<error_tolerance));
sprintf('correct number is %d out of 256, when error_tolerance is %d', correct_num, error_tolerance)
end
