passports = {}

passport = {}
for line in io.lines("input") do
   if line == "" then
         table.insert(passports, passport)
         passport = {}
      else
         for k, v in line:gmatch("(%w+):(%g+)") do passport[k] = v end
   end
end

valid = 0
fields = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"}
for i, passport in ipairs(passports) do
   print("\npassport", i)
   isvalid = true
   for _, field in ipairs(fields) do
      if not passport[field] then
         print(string.format("\tinvalid: does not have: %s", field))
         isvalid = false
      else
         print(string.format("\t%s: %s", field, passport[field]))
      end
   end
   if isvalid then valid = valid + 1 end
end
   
print(valid)
         

local function check_passport(passport)
   local fields = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"}
   local checkers = {
      byr = function(year)
         year = tonumber(year)
         return year and year >= 1920 and year <= 2002
      end,
      iyr = function(year)
         year = tonumber(year)
         return year and year >= 2010 and year <= 2020
      end,
      eyr = function(year)
         year = tonumber(year)
         return year and year >= 2020 and year <= 2030
      end,
      hgt = function(height)
         local number, units = height:match("(%d+)(%a%a)")
         if not number or not units then return end
         number = tonumber(number)
         if units == "cm" then return number >= 150 and number <= 193 end
         if units == "in" then return number >= 59 and number <= 76 end
      end,
      hcl = function(color)
         pattern = "#[0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f]"
         return color == color:match(pattern)
      end,
      ecl = function(color)
         colors = {"amb", "blu", "brn", "gry", "grn", "hzl", "oth"}
         is_in_colors = false
         for _, check in ipairs(colors) do
            if color == check then
               is_in_colors = true
               break
            end
         end
         return is_in_colors
      end,
      pid = function(number)
         pattern = string.rep("[0-9]", 9)
         return number == number:match(pattern)
      end
   }

   isvalid = true
   for _, field in ipairs(fields) do
      if not passport[field] then
         print(string.format("\tinvalid: missing field %s", field))
         isvalid = false
      elseif not checkers[field](passport[field]) then
         print(string.format("\tinvalid format for %s: %s", field, passport[field]))
         isvalid = false
      else
         print(string.format("\t%s: %s", field, passport[field]))
      end
   end

   return isvalid
end

valid = 0
for i, passport in ipairs(passports) do
   print("\npassport", i)
   isvalid = check_passport(passport)
   if isvalid then valid = valid + 1 end
end
print(valid)
   

   

