--[[
Roblox model containing all thing for this https://create.roblox.com/store/asset/17498981144
This script is only made for refrence and of course it made for roblox game engine.
]]

local webhook = "https://discordapp.com/api/webhooks/webhookidhere"


local HttpService = game:GetService("HttpService")
local PlayerService = game:GetService("Players")

local urlTarget = "-"-- im not gonna be dumb enough leaving this empty just fill it with your api link here

local event = game:GetService("ReplicatedStorage"):WaitForChild("Connection")

local function getPlayerName(plr)
	if plr.Name ~= plr.DisplayName then
		return plr.Name.."("..plr.DisplayName..")"
	else
		return plr.Name
	end
end

event.OnServerEvent:Connect(function(plr,msg)
	local Playername = getPlayerName(plr)
	
	local data = {
		["message"] = msg,
		["user"] = Playername,
		["webhook"] = webhook,
		["source"] = game:GetService("MarketplaceService"):GetProductInfo(game.PlaceId).Name
	}
	print(data)
	local jsondata = HttpService:JSONEncode(data)
	local response = HttpService:PostAsync(urlTarget,jsondata)
	
	warn(response)
	event:FireClient(plr,"request complete at my end idk about discord end tho")
end)
