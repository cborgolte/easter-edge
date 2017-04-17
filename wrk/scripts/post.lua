wrk.method = "POST"
wrk.headers["Content-Type"] = "application/json"
wrk.path = "/edges"


counter = 1
local threadId = 100000
local threads = {}

function setup(thread)
   thread:set("id", threadId)
   table.insert(threads, thread)
   threadId = threadId + 100000
end


request = function() 
    local val = id + counter
    body = '{"in": "in_' .. val .. '", "out": "out_' .. val .. '"}'
    counter = counter + 1
    return wrk.format(nil, nil, nil, body)
end
