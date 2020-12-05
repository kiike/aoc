trees = {}

lines = io.lines("input.txt")
for line in lines do table.insert(trees, line:rep(400)) end

sum = 0
x = 1
for y=1, #trees do
   if trees[y]:sub(x, x) == "#" then sum = sum + 1 end
   x = x + 3
end
print(sum)

sums = 1
sum = 0
x = 1
for y=1, #trees do
   if trees[y]:sub(x, x) == "#" then sum = sum + 1 end
   x = x + 3
end
sums = sums * sum

sum = 0
x = 1
for y=1, #trees do
   if trees[y]:sub(x, x) == "#" then sum = sum + 1 end
   x = x + 1
end
sums = sums * sum

sum = 0
x = 1
for y=1, #trees do
   if trees[y]:sub(x, x) == "#" then sum = sum + 1 end
   x = x + 5
end
sums = sums * sum

sum = 0
x = 1
for y=1, #trees do
   if trees[y]:sub(x, x) == "#" then sum = sum + 1 end
   x = x + 7
end
sums = sums * sum

sum = 0
x = 1
for y=1, #trees, 2 do
   if trees[y]:sub(x, x) == "#" then sum = sum + 1 end
   x = x + 1
end
sums = sums * sum

print(sums)
