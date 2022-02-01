package com.example.first_project;

import java.util.List;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;
@Repository
public interface ResultRepository extends JpaRepository<Result, Long>{
    
    List<Result> findByExamId(Long id);
    List<Result> findByStudent(String student);
}