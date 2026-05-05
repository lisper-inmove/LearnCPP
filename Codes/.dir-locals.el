((c++-mode . ((eval . (setq flycheck-clang-include-path
                            (list "/home/inmove/.opt/opencv_cuda/include/opencv4"
                                  "/usr/local/include")))
              (eval . (setq flycheck-gcc-include-path
                            (list "/home/inmove/.opt/opencv_cuda/include/opencv4"
                                  "/usr/local/include"))))))

((nil . ((eval . (setq projectile-project-compilation-cmd
                       (lambda ()
                         (let* ((build-dir (projectile-project-root))
                                (choice (completing-read "Build type: " '("Debug" "Release") nil t)))
                           (format "cd %s/build-%s && cmake --build ."
                                   (file-name-as-directory build-dir)
                                   (downcase choice)))))))))
