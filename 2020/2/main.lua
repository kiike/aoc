sum = 0
lines = io.lines("input.txt")

for line in lines do
   lower, upper, char, password = line:match("(%d+)%-(%d+) (%l): (%a+)")
   matches = 0
   for match in password:gmatch(char) do matches = matches + 1 end
   if matches >= tonumber(lower) and matches <= tonumber(upper) then
      sum = sum + 1
   end
end
print("part 1:", sum)

lines = io.lines("input.txt")
sum = 0
for line in lines do
   first, second, char, password = line:match("(%d+)%-(%d+) (%l): (%a+)")
   first = tonumber(first)
   second = tonumber(second)
   matches = 0
   if password:sub(first, first) == char then matches = matches + 1 end
   if password:sub(second, second) == char then matches = matches + 1 end
   if matches == 1 then sum = sum + 1 end
end
print("part 2:", sum)
