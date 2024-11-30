Import("env")

print("Current CLI targets", COMMAND_LINE_TARGETS)
print("Current Build targets", BUILD_TARGETS)

def before_upload(source, target, env):
    print("before_upload")
    # call Node.JS or other script
    env.Execute("cd web && pnpm i && pnpm build")

env.AddPreAction("upload", before_upload)

