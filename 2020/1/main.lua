input = {}
for line in io.lines("input.txt") do
   table.insert(input, tonumber(line))
end

for _, j in pairs(input) do
   for _, k in pairs(input) do
      for _, l in pairs(input) do
         if j+k+l == 2020 then
            print(j, k, l, j*k*l)
            break
         end
      end
   end
end
