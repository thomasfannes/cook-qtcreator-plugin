generated = "generated"
directory generated

desc "Clean"
task :clean do
    FileUtils.rm_rf(generated)
end

desc "Build"
task :build => generated do
    Dir.chdir(generated) do
        sh "qmake ../CookProjectManager.pro -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug IDE_SOURCE_TREE=qt-creator"
        sh "make qmake_all"
        sh "make -j12"
    end
end
