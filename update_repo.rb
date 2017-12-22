require 'git'

namespace :repo do
    src_commit = "v4.4.1"
    dst_commit = "v4.5.0"
    $repo_loc = File.join(Dir.pwd, "qt-creator")
    
    def get_commit()
        g = Git.open($repo_loc)
        g.gcommit('HEAD').sha
    end
    
    task :build do
        puts "Building #{get_commit}"
        
        begin
            Dir::chdir("build/debug") { sh 'make -j8' }
        rescue
            raise "Error building #{get_commit}"
        end
    end
    
    task :step_without_build do
        g = Git.open($repo_loc)
        commits = g.log.between("HEAD", dst_commit).to_a
        if commits.empty?
            puts "All commits processed, at #{dst_commit}"
        else
            c = commits[-1]
            Dir.chdir($repo_loc) { sh "git checkout #{c}" }
            puts "Still #{commits.size} left"
        end
    end
    
    task :step => [:build, :step_without_build] 
end
